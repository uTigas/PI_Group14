from django.db import models
from django.contrib.auth.models import User

class Organization(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)

class Role(models.Model):
    role = models.CharField(max_length=30)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)

class Member(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.SET_NULL, null=True)

class Owner(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)

class OrganizationVault(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)

class Vault(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)

class OrganizationHasVault(models.Model):
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    vault = models.ForeignKey(OrganizationVault, on_delete=models.CASCADE)

class Permission(models.TextChoices):
        VIEW = 'VIEW'
        CREATE = 'CREATE'
        EDIT = 'EDIT'

class RolePermission(models.Model):
    vault = models.ForeignKey(OrganizationVault, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.CASCADE)
    permission = models.CharField(max_length=50, choices=Permission.choices)

class MemberPermission(models.Model):
    vault = models.ForeignKey(OrganizationVault, on_delete=models.CASCADE)
    member = models.ForeignKey(Member, on_delete=models.CASCADE)
    permission = models.CharField(max_length=50, choices=Permission.choices)