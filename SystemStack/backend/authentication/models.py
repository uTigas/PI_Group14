from django.db import models
from django.contrib.auth.models import AbstractUser

# Create your models here.


class qeepUser(AbstractUser):
    username = models.CharField(max_length=50, primary_key=True)
    first_name = models.CharField(max_length=50)
    last_name = models.CharField(max_length=50)
    phone = models.CharField(max_length=20)
    email = models.CharField(max_length=70)
    first_name = models.CharField(max_length=50)
    
    def get_full_name(self):
        return f"{self.first_name} {self.last_name}"
    
class entry(models.Model):
    rx_id = models.CharField(max_length=30)
    user = models.ForeignKey(qeepUser, on_delete= models.CASCADE)
    