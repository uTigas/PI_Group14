"""
URL configuration for django_config project.

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/5.0/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.contrib import admin
from django.urls import path, re_path
from authentication import views as auth_views
from entities import views as ent_views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('register', auth_views.register, name="register"),
    re_path(r'^login/$', auth_views.systemLogin, name='login'),
    path('logout', auth_views.systemLogout, name="logout"),
    path('check-authentication', auth_views.check_authentication, name="check-authentication"),
    path('user', auth_views.fetch_user, name="user"),
    path('organizations/member', ent_views.getMemberOrganizations, name ="organizations/member"),
    path('organizations/owner', ent_views.getOwnedOrganizations, name ="organizations/owner"),
    path('organizations/create', ent_views.createOrganization, name ="organizations/create"),
]
