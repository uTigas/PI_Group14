from django.http import JsonResponse, HttpResponse
from django.shortcuts import render, redirect
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.decorators import login_required
from .forms import RegisterForm, LoginForm
from django.contrib.auth.models import User
from django.core import serializers


def check_authentication(request):
    if request.user.is_authenticated:
        return JsonResponse({'is_authenticated': True})
    else:
        return JsonResponse({'is_authenticated': False})

def register(response):
    form = RegisterForm()
    if response.method == "POST":
        form = RegisterForm(response.POST)
        if form.is_valid():
            form.save()
            return redirect('/login')
            
    return render(response, "register/register.html", {"form":form})

def systemLogin(request):
    next = request.GET.get('next')
    messages = []
    form = LoginForm()
    if request.method == "POST":
        form = LoginForm(request.POST)
        if form.is_valid():
            username = form.cleaned_data["username"]
            password = form.cleaned_data["password"]
            user = authenticate(request, username=username, password=password)
            if user is not None:
                login(request, user)
                if next == None:
                    response = redirect('http://localhost:8100')  
                else:
                    response = redirect(next)  
                return response  
        messages.append("Invalid Credentials!")
    return render(request, "login/login.html", {"form": form, "messages": messages})

@login_required
def systemLogout(request):
    logout(request)
    return redirect('http://localhost:8100')  

@login_required
def fetch_user(request):
    user = serializers.serialize("json", [User.objects.get(username = request.user)])
    response = HttpResponse(user)
    return response
