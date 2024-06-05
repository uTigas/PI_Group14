from django.views.decorators.csrf import csrf_exempt
from django.forms import model_to_dict
from django.http import JsonResponse, HttpResponse
from django.db import transaction
from django.shortcuts import render, redirect
from django.contrib.auth import authenticate, login, logout
from django.contrib.auth.decorators import login_required
from authentication.models import entry, qeepUser
from entities.models import Statistics
import authentication.comms.util as util
import authentication.comms.msg as msg
import requests

def check_authentication(request):
    if request.user.is_authenticated:
        return JsonResponse({'is_authenticated': True})
    else:
        #TODO: Implement solution for MITM Attack problem on return of auth session
        #Not Logged In? Proceed to login.

        next = request.POST.get('next')
        if request.method == "POST":
            id = request.POST.get["id"]
            challenge = request.POST.get["challenge"]
            
            #Go to Database and fetch User's key to check validity of message
            response = requests.post(util.get_qkd_server_address_() + "/keys/get/" + util.get_server_id_() + "/" + id,
                                    data = msg.GetKeyMsg.construct(util.get_server_id_(), id).encrypt(util.get_qkd_server_key_()))
            
            if response.status_code != 200:
                return JsonResponse({"error":"Invalid Credentials!"}, status = 401)
            
            try:
                user_key = str(msg.ReturnKeysMsg(response.data, util.get_qkd_server_key_()).loads().get(id))
            except:
                return JsonResponse({"error":"An Error Occurred decoding Message!"}, status = 401)
                
            #Check challenge
            if id != util.decrypt_msg(challenge, user_key):
                return JsonResponse({"error":"Invalid Credentials!"}, status = 401)
            
            if True:
                username = entry.objects.filter(rx_id=id).first()
                if not username:
                    return JsonResponse({"error":"No Account Created!"}, status = 401)

                user = authenticate(request, username=username)
                if user is not None:
                    login(request, user)
                    if next == None:
                        response = redirect('http://localhost:8100')  
                    else:
                        response = redirect(next)  
                    return response  
            
        return JsonResponse({'is_authenticated': False})
    
@csrf_exempt
def register(response):
    print("In register")
    if response.method == "POST":
        form = response.POST
        with transaction.atomic():
            qeepUser.objects.create(username= form['username'], first_name= form['first_name'], last_name= form['last_name'], email= form['email'], phone= form['phone'],)
            statistics = Statistics.objects.filter(id = 1).first()
            if not statistics:
                statistics = Statistics.objects.create(id = 1)
            statistics.users += 1
            statistics.save()
            return HttpResponse(status= 201)

@login_required
def systemLogout(request):
    logout(request)
    return redirect('http://localhost:8100')  

@login_required
def fetch_user(request):
    user = model_to_dict(qeepUser.objects.get(username = request.user))
    response = JsonResponse({"user":user})
    return response
