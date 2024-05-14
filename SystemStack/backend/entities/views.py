import json
from django.forms import model_to_dict
from django.http import HttpResponse, JsonResponse
from django.shortcuts import render
from django.contrib.auth.models import User
from entities.forms import OrganizationForm, OrganizationVaultForm
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
                    models.Role.objects.create(role = models.Default_Roles.USER, organization = organization)
                    models.Role.objects.create(role = models.Default_Roles.ADMIN, organization = organization)
                    ownerRole = models.Role.objects.create(role = models.Default_Roles.OWNER, organization = organization)
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
                roleAllowedVaults = models.RolePermission.objects.filter(organizationHasVault__organization__id = organizationId, role=member.role, permission=models.Permission.VIEW)
                specificAllowedVaults = models.MemberPermission.objects.filter(organizationHasVault__organization__id = organizationId, member=member, permission=models.Permission.VIEW)
                
                for m in tempMembers:
                    members.append({
                        "username": m.user.username,
                        "name": m.user.get_full_name(),
                        "role": m.role.role
                    })


                for v in roleAllowedVaults:
                    temp = {
                        "vault": model_to_dict(v.organizationHasVault.vault),
                        "permission": v.permission
                    }
                    if temp not in vaults: 
                        vaults.append(temp)

                for v in specificAllowedVaults:
                    temp = {
                        "vault": model_to_dict(v.organizationHasVault.vault),
                        "permission": v.permission
                    }
                    if temp not in vaults: 
                        vaults.append(temp)
                return JsonResponse({"role":role, "members": members, "vaults": vaults})

        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"An error occurred"}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)

@login_required
@csrf_exempt
def createOrganizationVault(request):
    if request.method == 'POST':
        try:
            with transaction.atomic():
                form = OrganizationVaultForm(request.POST)
                organization = models.Organization.objects.get(id = request.POST.get('organization'))

                userRole = models.Role.objects.get(organization = organization, role=models.Default_Roles.USER)
                adminRole = models.Role.objects.get(organization = organization, role=models.Default_Roles.ADMIN)
                ownerRole = models.Role.objects.get(organization = organization, role=models.Default_Roles.OWNER)
                if form.is_valid():
                    organizationVault = form.save()
                    organizationHasVault = models.OrganizationHasVault.objects.create(organization=organization, vault=organizationVault)
                    
                    #For each permission
                    for perm in models.Permission.choices:
                        print(perm[1])
                        if request.POST.get(perm[1]) == "Everyone":
                            models.RolePermission.objects.create(role=userRole, organizationHasVault = organizationHasVault, permission= perm[0])
                            models.RolePermission.objects.create(role=adminRole, organizationHasVault = organizationHasVault, permission= perm[0])
                            models.RolePermission.objects.create(role=ownerRole, organizationHasVault = organizationHasVault, permission= perm[0])
                    
                        elif request.POST.get(perm[1]) == "Admin":
                            models.RolePermission.objects.create(role=adminRole, organizationHasVault = organizationHasVault, permission= perm[0])
                            models.RolePermission.objects.create(role=ownerRole, organizationHasVault = organizationHasVault, permission= perm[0])
                    
                        elif request.POST.get(perm[1]) == "Just Me":
                            user = User.objects.get(username = request.user)
                            member = models.Member.objects.get(user=user, organization=organization)
                            models.RolePermission.objects.create(role=ownerRole, organizationHasVault = organizationHasVault, permission= perm[0])
                            models.MemberPermission.objects.create(member=member, organizationHasVault = organizationHasVault, permission= perm[0])

                

                    return HttpResponse(status=200)
                else:
                    return JsonResponse({'error': form.errors}, status=400)
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"An error occurred"}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)
