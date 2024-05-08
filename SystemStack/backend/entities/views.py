import json
from django.forms import model_to_dict
from django.http import HttpResponse, JsonResponse
from django.shortcuts import render
from django.contrib.auth.models import User
from entities.forms import OrganizationForm
from django.contrib.auth.decorators import login_required
from django.views.decorators.csrf import csrf_exempt
from django.db import transaction
from django.core.serializers import serialize

import entities.models as models
# Create your views here.

@login_required
def getOrganizations(request):
    if request.method == 'GET':
        try:
            user = User.objects.get(username = request.user)
            members = models.Member.objects.filter(user = user)
            organizations = []
            for member in members:
                organizations.append({
                    "organization": model_to_dict(member.organization),
                    "role": member.role.role
                })
            return JsonResponse({"organizations":  organizations})
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)

@login_required
@csrf_exempt
def createOrganization(request):
    if request.method == 'POST':
        try:
            user = User.objects.get(username = request.user)
            form = OrganizationForm(request.POST)
            if form.is_valid():
                with transaction.atomic():
                    organization = form.save()
                    models.Role.objects.create(role = "USER", organization = organization)
                    models.Role.objects.create(role = "ADMIN", organization = organization)
                    ownerRole = models.Role.objects.create(role = "OWNER", organization = organization)
                    models.Member.objects.create(user=user, organization=organization, role=ownerRole)
                return HttpResponse(status=200)
            else:
                return JsonResponse({'error': form.errors}, status=400)
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"An error occurred"}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)

@login_required
def getOrganizationDetails(request):
    if request.method == 'GET':
        try:
            organizationId = request.GET.get('id',None)
            user = User.objects.get(username = request.user)
            member  = models.Member.objects.filter(user = user, organization__id = organizationId).first()
            
            if not member:
                return JsonResponse({"error": "User not detected as part of selected Organization"}, status=400)
            else:
                vaults = []  
                members = []
                role = member.role.role
                tempMembers = models.Member.objects.filter(organization__id = organizationId)
                roleAllowedVaults = models.RolePermission.objects.filter(organizationHasVault__organization__id = organizationId, role=member.role, permission__in = ["VIEW", "CREATE", "EDIT"])
                specificAllowedVaults = models.MemberPermission.objects.filter(organizationHasVault__organization__id = organizationId, member=member, permission__in = ["VIEW", "CREATE", "EDIT"])
                
                for m in tempMembers:
                    members.append({
                        "username": m.user.username,
                        "name": m.user.get_full_name(),
                        "role": m.role.role
                    })


                for v in roleAllowedVaults:
                    vaults.append({
                        "vault": v.organizationHasVault.vault,
                        "permission": v.permission
                    })

                for v in specificAllowedVaults:
                    vaults.append({
                        "vault": v.organizationHasVault.vault,
                        "permission": v.permission
                    })
                    
                return JsonResponse({"role":role, "members": list(members), "vaults": list(vaults)})

        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"An error occurred"}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)
