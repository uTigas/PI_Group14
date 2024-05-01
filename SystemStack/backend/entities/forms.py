
from django import forms

from entities.models import Organization


class OrganizationForm(forms.ModelForm):
    class Meta:
        model = Organization
        fields = "__all__"