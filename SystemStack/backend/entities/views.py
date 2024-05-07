import json
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
def getOwnedOrganizations(request):
    try:
        user = User.objects.get(username = request.user)
        owners = models.Owner.objects.filter(user = user)
        organizations = []
        for owner in owners:
            print(owner)
            organizations.append(owner.organization)
        serialized_organizations = serialize('json', organizations)
        return JsonResponse({"organizations": serialized_organizations})
    except User.DoesNotExist:
        return JsonResponse({"error": "User does not exist"}, status=400)
    except Exception as e:
        return JsonResponse({"error": str(e)}, status=500)

@login_required
def getMemberOrganizations(request):
    try:
        user = User.objects.get(username = request.user)
        members = models.Member.objects.filter(user = user)
        organizations = []
        for member in members:
            organizations.append((member.organization, member.role))
        serialized_organizations = serialize('json', organizations)
        return JsonResponse({"organizations": serialized_organizations})
    except User.DoesNotExist:
        return JsonResponse({"error": "User does not exist"}, status=400)
    except Exception as e:
        return JsonResponse({"error": str(e)}, status=500)

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
                    owner = models.Owner.objects.create(user=user, organization=organization)
                return HttpResponse(status=200)
            else:
                return JsonResponse({'error': form.errors}, status=400)
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)

