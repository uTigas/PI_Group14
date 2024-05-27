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
from drf_yasg import openapi
from drf_yasg.views import get_schema_view as swagger_view

schema_view = swagger_view(
    openapi.Info(
        title="Qeep API",
        default_version = '1.0.0'
    )
)
urlpatterns = [
    path('doc', schema_view.with_ui('swagger', cache_timeout=0), name="swagger-schema"),
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
    path('vault/upload', war_views.uploadFile, name ="vault/upload"),
    path('vault/fetch', war_views.getFile, name ="vault/fetch"),
    path('vault/update', war_views.renameFile, name ="vault/update"),
    path('vault/delete', war_views.deleteFile, name ="vault/delete"),
    path('organization-vault', ent_views.getOrganizationVaultDetails, name ="organization-vault"),
    path('vault', ent_views.getVaultItems, name ="vault"),
    path('stats', ent_views.getStatistics, name ="stats"),
    path('chats', ent_views.getChats, name ="chats"),
    path('chats/invite', ent_views.inviteChat, name ="chats/invite"),
    path('chats/invites', ent_views.getChatInvites, name ="chats/invites"),
    path('chats/invite/accept', ent_views.acceptChatInvite, name ="chats/invite/accept"),
    path('chats/invite/refuse', ent_views.refuseChatInvite, name ="chats/invite/refuse"),
    path('chat', ent_views.getChat, name ="chat"),
    path('chat/message', ent_views.sendMessage, name ="chat/message"),
]
