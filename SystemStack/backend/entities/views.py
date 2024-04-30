from django.http import JsonResponse
from django.shortcuts import render
from django.contrib.auth.models import User
from backend.entities.forms import OrganizationForm
import entities.models as models
# Create your views here.

def getOwnedOrganizations(request):
    user = User.objects.get(username = request.user)
    owners = models.Owner.objects.filter(user = user).values()
    organizations = []
    for owner in owners:
        organizations.append(owner.organization)
    return JsonResponse(organizations)

def getMemberOrganizations(request):
    user = User.objects.get(username = request.user)
    members = models.Member.objects.filter(user = user).values()
    organizations = []
    for member in members:
        organizations.append((member.organization, member.role))
    return JsonResponse(organizations)

def createOrganization(request):
    if request.method == 'POST':
        form = OrganizationForm(request.POST)
        if form.is_valid():
            organization = models.Organization()
            organization.name = form.name
            organization.description = form.description
            organization.save()
            return JsonResponse(organization)
        else:
            return JsonResponse({'error': form.errors}, status=400)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)

