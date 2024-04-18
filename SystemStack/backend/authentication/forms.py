from django import forms
from django.contrib.auth import login, authenticate
from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.models import User

class RegisterForm(UserCreationForm):
    email = forms.EmailField()
    name = forms.Field()
    phone = forms.Field()

    class Meta:
        model = User
        fields = ["username", "name", "email", "phone", "password1", "password2"]