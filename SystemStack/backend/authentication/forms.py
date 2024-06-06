from django import forms
from django.contrib.auth.forms import UserCreationForm
from authentication.models import qeepUser

class RegisterForm(UserCreationForm):
    email = forms.EmailField()
    first_name = forms.CharField()
    last_name = forms.CharField()
    phone = forms.Field()

    class Meta:
        model = qeepUser
        fields = ["username", "first_name", "last_name", "email", "phone"]

class LoginForm(forms.Form):
    username = forms.CharField()
