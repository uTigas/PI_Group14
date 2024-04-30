
from django import forms


class OrganizationForm(forms.Form):
    name = forms.CharField()
    description = forms.CharField()