from django.db import models
from django.contrib.auth.models import User


class Organization(models.Model):
    description = models.CharField(300)

class OrganizationVault(models.Model):
    name = models.CharField(max_length=50)
    description = models.CharField(max_length=300)

class Owner(models.Model):
    user = models.ForeignKey(User, on_delete=models.SET_NULL, null=True)
    organization = models.ForeignKey(Organization, on_delete=models.SET_NULL, null=True),
    vault = models.ForeignKey(Vault, on_delete=models.SET_NULL)
