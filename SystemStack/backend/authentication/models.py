from django.db import models

# Create your models here.


class qeepUser(models.Model):
    username = models.CharField(max_length=50, primary_key=True)
    first_name = models.CharField(max_length=50)
    last_name = models.CharField(max_length=50)
    phone = models.CharField(max_length=20)
    email = models.CharField(max_length=70)
    first_name = models.CharField(max_length=50)
    
    def get_full_name(self):
        return f"{self.first_name} {self.last_name}"