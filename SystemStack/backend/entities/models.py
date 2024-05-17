from django.db import models
from django.contrib.auth.models import User

class Size(models.TextChoices):
    SMALL = 'small'
    MEDIUM = 'medium' 
    LARGE = 'large'

class Organization(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)
    size = models.CharField(choices=Size.choices, max_length=10, default=Size.SMALL)

class Default_Roles(models.TextChoices):
    USER = 'USER'
    ADMIN = 'ADMIN'
    OWNER = 'OWNER'


class Role(models.Model):
    role = models.CharField(max_length=30)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)

class Member(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    joined = models.DateTimeField(auto_now_add=True)

class MemberHasRole(models.Model):
    member = models.ForeignKey(Member, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.CASCADE)

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
    VIEW = 'VIEW'
    EDIT = 'EDIT'
    MANAGE = 'MANAGE'
    MEMBERS = 'MEMBERS'
    CREATE_VAULTS = 'CREATE_VAULTS'
    MANAGE_MEMBERS = 'MANAGE_MEMBERS'
    MANAGE_ORGANIZATION = 'MANAGE_ORGANIZATION'

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
    PENDING = "PENDING"
    ACCEPTED = "ACCEPTED"
    REFUSED = "REFUSED"

class MemberInvite(models.Model):
    inviter = models.ForeignKey(User, related_name='inviter', on_delete=models.CASCADE)
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.SET_NULL, null=True)
    timestamp = models.DateTimeField(auto_now_add=True)
    status = models.CharField(max_length=20, choices=Status.choices, default=Status.PENDING)