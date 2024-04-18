from django.shortcuts import render, redirect
from .forms import RegisterForm
# Create your views here.

def register(response):
    form = RegisterForm()
    if response.method == "POST":
        form = RegisterForm(response.POST)
        if form.is_valid():
            form.save()
            return redirect('http://localhost:8100')
            
    return render(response, "register/register.html", {"form":form})