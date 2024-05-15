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
            return JsonResponse({"organizations":  organizations}, status=200)
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
                    userRole = models.Role.objects.create(role = models.Default_Roles.USER, organization = organization)
                    adminRole = models.Role.objects.create(role = models.Default_Roles.ADMIN, organization = organization)
                    ownerRole = models.Role.objects.create(role = models.Default_Roles.OWNER, organization = organization)
                    models.Member.objects.create(user=user, organization=organization, role=ownerRole)
                    models.RoleOrganizationPermission.objects.create(role = adminRole, permission=models.Permission.CREATE_VAULTS)
                    models.RoleOrganizationPermission.objects.create(role = adminRole, permission=models.Permission.MANAGE_MEMBERS)
                    models.RoleOrganizationPermission.objects.create(role = ownerRole, permission=models.Permission.CREATE_VAULTS)
                    models.RoleOrganizationPermission.objects.create(role = ownerRole, permission=models.Permission.MANAGE_MEMBERS)
                    models.RoleOrganizationPermission.objects.create(role = ownerRole, permission=models.Permission.MANAGE_ORGANIZATION)

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
            organization = models.Organization.objects.get(id = request.GET.get('organization'))
            user = User.objects.get(username = request.user)
            member  = models.Member.objects.filter(user = user, organization = organization).first()
            
            if not member:
                return JsonResponse({"error": "User not detected as part of selected Organization"}, status=400)
            else:
                vaults = []  
                members = []
                role = member.role
                permissions = models.RoleOrganizationPermission.objects.filter(role=role).values_list('permission', flat=True)
                tempMembers = models.Member.objects.filter(organization = organization)
                roleAllowedVaults = models.RoleVaultPermission.objects.filter(organizationHasVault__organization = organization, role=member.role, permission=models.Permission.VIEW)
                specificAllowedVaults = models.MemberPermission.objects.filter(organizationHasVault__organization = organization, member=member, permission=models.Permission.VIEW)
                
                for m in tempMembers:
                    members.append({
                        "username": m.user.username,
                        "name": m.user.get_full_name(),
                        "role": m.role.role,
                        "joined": m.joined
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
                return JsonResponse({"organization": model_to_dict(organization), "role":role.role,"permissions": list(permissions), "members": members, "vaults": vaults}, status=200)

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
            organization = models.Organization.objects.get(id = request.POST.get('organization'))
            user = User.objects.get(username = request.user)
            member  = models.Member.objects.filter(user = user, organization = organization).first()
            
            #Verify if allowed
            if not models.Permission.CREATE_VAULTS in list(models.RoleOrganizationPermission.objects.filter(role = member.role).values_list('permission', flat=True)):
                return JsonResponse({"error":"User not allowed to create vaults in this organization."}, status=403)
            with transaction.atomic():
                form = OrganizationVaultForm(request.POST)

                userRole = models.Role.objects.get(organization = organization, role=models.Default_Roles.USER)
                adminRole = models.Role.objects.get(organization = organization, role=models.Default_Roles.ADMIN)
                ownerRole = models.Role.objects.get(organization = organization, role=models.Default_Roles.OWNER)
                if form.is_valid():
                    organizationVault = form.save()
                    organizationHasVault = models.OrganizationHasVault.objects.create(organization=organization, vault=organizationVault)
                    
                    #For each permission
                    for perm in models.Permission.choices:
                        if request.POST.get(perm[1]) == "Everyone":
                            models.RoleVaultPermission.objects.create(role=userRole, organizationHasVault = organizationHasVault, permission= perm[0])
                            models.RoleVaultPermission.objects.create(role=adminRole, organizationHasVault = organizationHasVault, permission= perm[0])
                            models.RoleVaultPermission.objects.create(role=ownerRole, organizationHasVault = organizationHasVault, permission= perm[0])
                    
                        elif request.POST.get(perm[1]) == "Admin":
                            models.RoleVaultPermission.objects.create(role=adminRole, organizationHasVault = organizationHasVault, permission= perm[0])
                            models.RoleVaultPermission.objects.create(role=ownerRole, organizationHasVault = organizationHasVault, permission= perm[0])
                    
                        elif request.POST.get(perm[1]) == "Just Me":
                            user = User.objects.get(username = request.user)
                            member = models.Member.objects.get(user=user, organization=organization)
                            models.RoleVaultPermission.objects.create(role=ownerRole, organizationHasVault = organizationHasVault, permission= perm[0])
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

@login_required
@csrf_exempt
def inviteMember(request):
    if request.method == 'POST':
        try:
            organization = models.Organization.objects.get(id = request.POST.get('organization'))
            user = User.objects.get(username = request.user)
            member  = models.Member.objects.filter(user = user, organization = organization).first()
            invitedUser = User.objects.get(username = request.POST.get("username"))
            #Verify if allowed
            if not models.Permission.MANAGE_MEMBERS in list(models.RoleOrganizationPermission.objects.filter(role = member.role).values_list('permission', flat=True)):
                return JsonResponse({"error":"User not allowed to manage members of this organization."}, status=403)

            #Is it already invited or member?
            if models.Member.objects.filter(user = invitedUser, organization = organization) or models.MemberInvite.objects.filter(user=invitedUser, organization = organization, status =models.Status.PENDING):
                return JsonResponse({"error":"User is already a member of invited."}, status=403)

            with transaction.atomic():
                models.MemberInvite.objects.create(inviter=user, user=invitedUser, organization=organization)
            return HttpResponse(status=200)
           
        except User.DoesNotExist:
            return JsonResponse({"error": "This user does not exist"}, status=404)
        except Exception as e:
            return JsonResponse({"An error occurred"}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)
    
@login_required
def getInvites(request):
    #DISCLAIMER: returns ONLY PENDING invites
    if request.method == 'GET':
        try:
            user = User.objects.get(username = request.user)
            invites_raw = models.MemberInvite.objects.filter(user = user)
            invites = []
            for invite in invites_raw:
                if invite.status != models.Status.PENDING:
                    continue
                temp = {
                    "id": invite.id,
                    "inviter": invite.inviter.first_name + " " + invite.inviter.last_name,
                    "organization": invite.organization.name,
                    "timestamp": invite.timestamp
                }
                invites.append(temp)
            return JsonResponse({"invites": invites}, status=200)
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)

@login_required
@csrf_exempt
def acceptInvite(request):
    if request.method == 'POST':
        try:
            user = User.objects.get(username = request.user)
            invite = models.MemberInvite.objects.get(id = request.POST.get("invite"))
            #Verify if allowed
            if not invite.user == user:
                return JsonResponse({"error":"Invite was not aimed to User."}, status=403)
            with transaction.atomic():
                invite.status = models.Status.ACCEPTED
                invite.save()
                userRole = models.Role.objects.get(organization = invite.organization, role = models.Default_Roles.USER)
                models.Member.objects.create(user = user, organization = invite.organization, role = userRole)
            return HttpResponse(status=200)
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)

@login_required
@csrf_exempt
def refuseInvite(request):
    if request.method == 'POST':
        try:
            user = User.objects.get(username = request.user)
            invite = models.MemberInvite.objects.get(id = request.POST.get("invite"))
            #Verify if allowed
            if not invite.user == user:
                return JsonResponse({"error":"Invite was not aimed to User."}, status=403)
            with transaction.atomic():
                invite.update(status = models.Status.REFUSED)
            return HttpResponse(status=200)
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)
    

@login_required
@csrf_exempt
def createRole(request):
    if request.method == 'POST':
        try:
            user = User.objects.get(username = request.user)
            organization = models.Organization.objects.get(id = request.POST.get('organization'))
            member = models.Member.objects.get(user = user, organization = organization)
            desiredRole = request.POST.get('role')
            #Verify if allowed
            if not (models.Member.objects.filter(organization = organization, user = user) and models.Permission.MANAGE_ORGANIZATION in list(models.RoleOrganizationPermission.objects.filter(role = member.role).values_list('permission', flat=True))):
                return JsonResponse({"error":"User not allowed to manage members of this organization."}, status=403)
            print(desiredRole)
            #Is there a Role with same name?
            if models.Role.objects.filter(organization = organization, role = desiredRole):
                return JsonResponse({"error":"That role is already created."}, status=403)

            with transaction.atomic():
                models.Role.objects.create(organization=organization, role=desiredRole)
            return HttpResponse(status=200)
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)
    
@login_required
@csrf_exempt
def updateRole(request):
    if request.method == 'POST':
        try:
            user = User.objects.get(username = request.user)
            organization = models.Organization.objects.get(id = request.POST.get('organization'))
            desiredPermissions = request.POST.get('permissions')
            role = models.Role.objects.get(id=request.POST.get('role'))

            #Verify if allowed
            if role == models.Default_Roles.OWNER or not (models.Member.objects.filter(organization = organization, user = user) and models.Permission.MANAGE_ORGANIZATION in list(models.RoleOrganizationPermission.objects.filter(role = user.role).values_list('permission', flat=True))):
                return JsonResponse({"error":"User not allowed to manage members of this organization."}, status=403)

            #Is there a Role with same name?
            if not models.Role.objects.filter(organization = organization, role = role):
                return JsonResponse({"error":"That role does not exist."}, status=403)

            with transaction.atomic():
                currentPermissions = models.RoleOrganizationPermission.objects.filter(role=role)
                toBeDeleted=[]
                toBeAdded=[]
                
                for perm in currentPermissions:
                    if perm.permission not in desiredPermissions:
                        toBeDeleted.append(perm)

               
                for perm in desiredPermissions:
                    save = False
                    for cPerm in currentPermissions:
                        if perm == cPerm.permission:
                            break

                    if save:
                        toBeAdded.append(perm)
                
                for perm in toBeDeleted:
                    perm.delete()
                
                for perm in toBeAdded:
                    models.RoleOrganizationPermission.objects.create(role=role, permission = perm)
                
            return HttpResponse(status=200)
        
        except User.DoesNotExist:
            return JsonResponse({"error": "User does not exist"}, status=400)
        except Exception as e:
            return JsonResponse({"error": str(e)}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)
    