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

class Role(models.Model):
    role = models.CharField(max_length=30)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)

class Member(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.SET_NULL, null=True)


class Vault(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)

class OrganizationVault(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)

class OrganizationHasVault(models.Model):
    organization = models.ForeignKey(Organization, on_delete=models.CASCADE)
    vault = models.ForeignKey(OrganizationVault, on_delete=models.CASCADE)

class Permission(models.TextChoices):
    VIEW = 'VIEW', 'View'
    CREATE = 'CREATE', 'Create'
    EDIT = 'EDIT', 'Edit'

class RolePermission(models.Model):
    organizationHasVault = models.ForeignKey(OrganizationHasVault, on_delete=models.CASCADE)
    role = models.ForeignKey(Role, on_delete=models.CASCADE)
    permission = models.CharField(max_length=50, choices=Permission.choices)

class MemberPermission(models.Model):
    organizationHasVault = models.ForeignKey(OrganizationHasVault, on_delete=models.CASCADE)
    member = models.ForeignKey(Member, on_delete=models.CASCADE)
    permission = models.CharField(choices=Permission.choices, max_length=10, default=Permission.VIEW)