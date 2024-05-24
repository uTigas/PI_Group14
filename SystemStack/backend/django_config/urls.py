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
from warehouse import views as war_views

urlpatterns = [
    path('admin/', admin.site.urls),
    path('register', auth_views.register, name="register"),
    re_path(r'^login/$', auth_views.systemLogin, name='login'),
    path('logout', auth_views.systemLogout, name="logout"),
    path('check-authentication', auth_views.check_authentication, name="check-authentication"),
    path('user', auth_views.fetch_user, name="user"),
    path('organizations', ent_views.getOrganizations, name ="organizations"),
    path('organization/create', ent_views.createOrganization, name ="organization/create"),
    path('organization', ent_views.getOrganizationDetails, name ="organization"),
    path('organization-vault/create', ent_views.createOrganizationVault, name ="organization-vault/create"),
    path('organization/members/invite', ent_views.inviteMember, name ="organization/members/invite"),
    path('user/invites', ent_views.getInvites, name ="user/invites"),
    path('user/invites/accept', ent_views.acceptInvite, name ="user/invites/accept"),
    path('user/invites/refuse', ent_views.refuseInvite, name ="user/invites/refuse"),
    path('organization/role/create', ent_views.createRole, name ="organization/role/create"),
    path('organization-vault/upload', war_views.uploadFile, name ="organization-vault/upload"),
    path('organization-vault/fetch', war_views.getFile, name ="organization-vault/fetch"),
    path('organization-vault', ent_views.getOrganizationVaultDetails, name ="organization-vault"),

]
