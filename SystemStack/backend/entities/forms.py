
from django import forms

from entities.models import Organization, OrganizationVault


class OrganizationForm(forms.ModelForm):
    class Meta:
        model = Organization
        fields = "__all__"

class OrganizationVaultForm(forms.ModelForm):
    class Meta:
        model = OrganizationVault
        fields = "__all__"