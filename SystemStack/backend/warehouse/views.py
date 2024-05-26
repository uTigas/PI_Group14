
import os
from django.contrib.auth.decorators import login_required
from django.views.decorators.csrf import csrf_exempt
from django.http import FileResponse, HttpResponse, JsonResponse
from entities.models import OrganizationVault, Statistics, User
from warehouse.models import Item
from django.forms import model_to_dict
from django.db import transaction

@csrf_exempt
def uploadFile(request):
    if request.method == 'POST':
        user = User.objects.get(username = request.user)
        with transaction.atomic():
            file = request.FILES['file']
            fileName = request.POST.get("fileName")
            vaultId = request.POST.get("vaultId")
            file_size = file.size
            file_type = file.name.split('.')[1]
            if vaultId != "":
                vault = OrganizationVault.objects.get(id = vaultId)
                if (vault):
                    item = Item(file=file, name=fileName, vault = vaultId, size = file_size, type = file_type, author = user.username)
                    item.save(using='warehouse')
                    
            else:
                item = Item(file=file, name=fileName, vault = user.username, size = file_size, type = file_type, author = user.username)
                item.save(using='warehouse')
            statistics = Statistics.objects.get(id = 1)
            statistics.items += 1
            statistics.size += round(file_size/1000/(1024*2))
            statistics.save()
        return HttpResponse('File uploaded successfully.', status = 200)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status = 405)

def getFile(request):
    if request.method == 'GET':
        try:
            id = request.GET.get('id')
            item = Item.objects.get(id=id)
            file_path = item.file.path
            return FileResponse(open(file_path, 'rb'), as_attachment=True, status = 200)
        except Item.DoesNotExist:
            return JsonResponse({"error": "Item not found"}, status=404)
        except Exception as e:
            return JsonResponse({"error": "An error occurred"}, status=500)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)

@csrf_exempt
def deleteFile(request):
    if request.method == 'POST':
        item_id = request.POST.get('id')
        try:
            item = Item.objects.get(id=item_id)
            file_path = item.file.path
            item.delete()
            if os.path.exists(file_path):
                os.remove(file_path)
                return JsonResponse({'message': 'Item and file deleted successfully'}, status=200)
            else:
                return JsonResponse({'error': 'File does not exist'}, status=404)
        except Item.DoesNotExist:
            return JsonResponse({'error': 'Item not found'}, status=404)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)
    
    
@csrf_exempt
def renameFile(request):
    if request.method == 'POST':
        item_id = request.POST.get('id')
        new_name = request.POST.get('name')
        try:
            item = Item.objects.get(id=item_id)
            item.name = new_name
            item.save()
            return HttpResponse({'message': 'File renamed successfully'}, status=200)
        except Item.DoesNotExist:
            return JsonResponse({'error': 'Item not found'}, status=404)
    else:
        return JsonResponse({'error': 'Method not allowed'}, status=405)