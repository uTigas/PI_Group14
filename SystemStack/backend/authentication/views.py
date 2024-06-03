from django.views.decorators.csrf import csrf_exempt
from django.forms import model_to_dict
from django.http import JsonResponse, HttpResponse
from django.db import transaction
from django.shortcuts import render, redirect
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.decorators import login_required
from .forms import LoginForm
from authentication.models import qeepUser
from entities.models import Statistics

def check_authentication(request):
    if request.user.is_authenticated:
        return JsonResponse({'is_authenticated': True})
    else:
        return JsonResponse({'is_authenticated': False})
    
@csrf_exempt
def register(response):
    print("In register")
    if response.method == "POST":
        print(response.POST)
        form = response.POST
        with transaction.atomic():
            qeepUser.objects.create(username= form['username'], first_name= form['first_name'], last_name= form['last_name'], email= form['email'], phone= form['phone'],)
            statistics = Statistics.objects.get(id = 1)
            statistics.users += 1
            statistics.save()
            print("AQui")
            return HttpResponse(status= 201)
@csrf_exempt
def systemLogin(request):
    next = request.GET.get('next')
    messages = []
    print("Django: System Login")
    if request.method == "POST":
        form = LoginForm(request.POST)
        if form.is_valid():
            username = form.cleaned_data["username"]

            #TODO: check body. challenge passes, auth user | Get key from QKD, decrypt, check, auth/refuse
            user = authenticate(request, username=username)
            if user is not None:
                login(request, user)
                if next == None:
                    response = redirect('http://localhost:8100')  
                else:
                    response = redirect(next)  
                return response  
        messages.append("Invalid Credentials!")

@login_required
def systemLogout(request):
    logout(request)
    return redirect('http://localhost:8100')  

@login_required
def fetch_user(request):
    user = model_to_dict(qeepUser.objects.get(username = request.user))
    response = JsonResponse({"user":user})
    return response
