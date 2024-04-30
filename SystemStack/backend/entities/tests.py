from django.test import TestCase

# Create your tests here.

from entities.models import RolePermission, Role, Organization, OrganizationVault, Permission

new_organization_vault = OrganizationVault()

new_organization_vault.name = "Test Vault"
new_organization_vault.description = "Description of the new vault"

new_organization_vault.save()

new_organization = Organization()

new_organization.name = "Test Organization"
new_organization.description = "Description of the new"

new_organization.save()

new_role = Role()

new_role.name = "Test Role"
new_role.organization = new_organization
new_role.save()

new_pem = RolePermission()

new_pem.vault = new_organization_vault
new_pem.role = new_role
new_pem.permission = "Test"
new_pem.save()

