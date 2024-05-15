from django.db import models
from django.contrib.auth.models import User

class Size(models.TextChoices):
    SMALL = 'small', 'Small'
    MEDIUM = 'medium', 'Medium'
    LARGE = 'large', 'Large'

class Organization(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)
    size = models.CharField(choices=Size.choices, max_length=10, default=Size.SMALL)

class Default_Roles(models.TextChoices):
    USER = 'USER', 'User'
    ADMIN = 'ADMIN', 'Admin'
    OWNER = 'OWNER', 'Owner'


class Role(models.Model):
    role = models.CharField(max_length=30)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)

class Member(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.SET_NULL, null=True)
    joined = models.DateTimeField(auto_now_add=True)

class Vault(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)

class OrganizationVault(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300, null=True)

class OrganizationHasVault(models.Model):
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    vault = models.ForeignKey(OrganizationVault, on_delete=models.CASCADE)

class Permission(models.TextChoices):
    VIEW = 'VIEW', 'view'
    EDIT = 'EDIT', 'edit'
    MANAGE = 'MANAGE', 'manage'
    MEMBERS = 'MEMBERS', 'members'
    CREATE_VAULTS = 'CREATE_VAULTS', 'create_vaults'
    MANAGE_MEMBERS = 'MANAGE_MEMBERS', 'manage_members'
    MANAGE_ORGANIZATION = 'MANAGE_ORGANIZATION', 'manage_organization'

class RoleVaultPermission(models.Model):
    organizationHasVault = models.ForeignKey(OrganizationHasVault, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.CASCADE)
    permission = models.CharField(max_length=50, choices=Permission.choices)

class RoleOrganizationPermission(models.Model):
    role = models.ForeignKey(Role, on_delete=models.CASCADE)
    permission = models.CharField(max_length=50, choices=Permission.choices)

class MemberPermission(models.Model):
    organizationHasVault = models.ForeignKey(OrganizationHasVault, on_delete=models.CASCADE)
    member = models.ForeignKey(Member, on_delete=models.CASCADE)
    permission = models.CharField(choices=Permission.choices, max_length=50, default=Permission.VIEW)

class Status(models.TextChoices):
    PENDING = "PENDING", "pending"
    ACCEPTED = "ACCEPTED", "accepted"
    REFUSED = "REFUSED", "refused"

class MemberInvite(models.Model):
    inviter = models.ForeignKey(User, related_name='inviter', on_delete=models.CASCADE)
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.SET_NULL, null=True)
    timestamp = models.DateTimeField(auto_now_add=True)
    status = models.CharField(max_length=20, choices=Status.choices, default=Status.PENDING)