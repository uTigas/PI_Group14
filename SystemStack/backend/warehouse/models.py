from django.db import models

class Item(models.Model):
    author = models.CharField(max_length=50)
    vault = models.CharField()
    name = models.CharField(max_length=50)
    file = models.FileField(upload_to='items/')
    size = models.IntegerField()
    type = models.CharField(max_length=10)
    createdAt = models.DateTimeField(auto_now_add=True)
    class Meta:
        app_label = 'warehouse'