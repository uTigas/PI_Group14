import axios, { AxiosResponse } from "axios";
import "../comms/client";
import { saveToLocalStorage, requestKey, getKeys, checkIfRegistered, checkIfRegisteredAndRegister, SELF_ID, SERVER_ID, encrypt, encryptMessage, decrypt, decryptMessage} from "../comms/client";

let one = false;
let key_cache: { [key: string]: string } = {}
const ApiWrapper = {
    backendURI : 'http://localhost:8000/',

    register: async (formData : FormData) => {
      try {
        formData.append('id', SELF_ID)
        const response = await axios.post(ApiWrapper.backendURI + "register", formData, {
          withCredentials: true
        });
        return response;
      } catch (error) {
        return error;
      }
    },

    checkAuthentication : async () => {
        try {
          if (!one) {
            one = true;
            await checkIfRegisteredAndRegister();
            const s = await requestKey(SERVER_ID);
            console.log("key" + s);

            // wait 1 second
            await new Promise(resolve => setTimeout(resolve, 1000));
            const r = await getKeys();
            const challenge = encryptMessage(SELF_ID, r['keys'][SERVER_ID].trim());
            const formData = new FormData;
            formData.append('id', SELF_ID)
            formData.append('challenge', challenge)
            return await axios.post(ApiWrapper.backendURI + 'check-authentication', formData, {withCredentials: true});
          }
        } catch (error) {
          console.error('Error checking authentication:', error);
        }
    },
    
    fetchUserDetails : async () => {
        try{
          return await axios.get(ApiWrapper.backendURI + 'user', {withCredentials: true});
        } catch (error){
          console.error('Error fetching User details', error);
        }
    },

    fetchOrganizations : async() => {
        try{
          let orgs = []
          return await axios.get(ApiWrapper.backendURI + 'organizations', {withCredentials: true});
        } catch (error){
          console.error('Error fetching User Organizations', error);
        }
      },

    fetchOrganizationDetails : async (organizationId : string) => {
      try{
        return await axios.get(ApiWrapper.backendURI + 'organization?organization=' + organizationId, {withCredentials: true});
      } catch (error){
        console.error('Error fetching Organization details', error);
      }
    },

    createVault: async (formData : URLSearchParams) => {
      await axios.post(ApiWrapper.backendURI + "organization-vault/create", formData, {
        withCredentials: true, 
        headers: {
        'Content-Type': 'application/x-www-form-urlencoded'
        }
      })
      .then(response => {})
      .catch(error => console.log(error))
    },

    inviteMember: async (formData : URLSearchParams) => {
      try {
        const response = await axios.post(ApiWrapper.backendURI + "organization/members/invite", formData, {
          withCredentials: true, 
          headers: {
            'Content-Type': 'application/x-www-form-urlencoded'
          }
        });
        return response;
      } catch (error) {
        return error;
      }
    },

    createOrganization: async (formData:URLSearchParams) => {
      await axios.post(ApiWrapper.backendURI + "organization/create", formData, {
        withCredentials: true, 
        headers: {
        'Content-Type': 'application/x-www-form-urlencoded'
        }
      })
      .then(response => {})
      .catch(error => console.log(error))
  
    },

    fetchInvites: async () => {
      try{
        return await axios.get(ApiWrapper.backendURI + 'user/invites', {withCredentials: true});
      } catch (error){
        console.error('Error fetching User Invites', error);
      }
    },

    fetchStats: async () => {
      try{
        return await axios.get(ApiWrapper.backendURI + 'stats', {withCredentials: true});
      } catch (error){
        console.error('Error fetching Stats', error);
      }
    },

    acceptInvite: async (formData : URLSearchParams) => {
      try {
        return await axios.post(ApiWrapper.backendURI + 'user/invites/accept', formData, {withCredentials: true});
      } catch (error) {
        console.error('Error accepting Invite', error);
      }
    },

    refuseInvite: async (formData : URLSearchParams) => {
      try {
        return await axios.post(ApiWrapper.backendURI + 'user/invites/refuse', formData, {withCredentials: true});
      } catch (error) {
        console.error('Error refusing Invite', error);
      }
    },

    createRole: async (formData : URLSearchParams) => {
      try {
        return await axios.post(ApiWrapper.backendURI + 'organization/role/create', formData, {withCredentials: true});
      } catch (error) {
        console.error('Error creating Role', error);
      }
    },

    uploadFile : async (formData : FormData,  setProgress: any) => {
      try {
        return await axios.post(ApiWrapper.backendURI + 'vault/upload', formData, {
            withCredentials: true,
            headers: {
              'Content-Type': 'multipart/form-data',
            },
            onUploadProgress: (progressEvent) => {
              const progress = Math.round((progressEvent.loaded / progressEvent.total) * 100);
              setProgress({progress:progress});
            },
          });
        } catch (error) {
          console.log("Error uploading File: " + error)
      }
    },

    fetchOrganizationVaultDetails: async (id: string) => {
      return await axios.get(ApiWrapper.backendURI + `organization-vault?vaultId=${id}`, {withCredentials: true});
    },

    downloadItem: async (id: string) => {
      try{
        return await axios.get(ApiWrapper.backendURI + `vault/fetch?id=${id}`, {responseType: 'blob', withCredentials: true});
      } catch (error){
        console.error('Error fetching User Invites', error);
      }
    },

    deleteItem:async (formData : FormData) => {
      try{
        return await axios.post(ApiWrapper.backendURI + "vault/delete" , formData, {withCredentials: true});
      } catch (error){
        console.error('Error deleting Item', error);
      }
    },

    renameItem:async (formData : FormData) => {
      try{
        return await axios.post(ApiWrapper.backendURI + "vault/update" , formData, {withCredentials: true});
      } catch (error){
        console.error('Error deleting Item', error);
      }
    },

    fetchVaultItems : async() => {
      try{
        return await axios.get(ApiWrapper.backendURI + 'vault', {withCredentials: true});
      } catch (error){
        console.error('Error fetching Vault Items', error);
      }
    },

    inviteChat: async (formData : FormData) => {
      return await axios.post(ApiWrapper.backendURI + "chats/invite", formData, {
          withCredentials: true
        });
    },

    acceptChatInvite: async (formData : FormData) => {
      return await axios.post(ApiWrapper.backendURI + 'chats/invite/accept', formData, {withCredentials: true});
    },

    refuseChatInvite: async (formData : FormData) => {
      return await axios.post(ApiWrapper.backendURI + 'chats/invite/refuse', formData, {withCredentials: true});
    },

    fetchContacts : async() => {
      return await axios.get(ApiWrapper.backendURI + 'chats', {withCredentials: true});
    },

    fetchChatInvites: async () => {
      return await axios.get(ApiWrapper.backendURI + 'chats/invites', {withCredentials: true});
    },

    fetchMessages: async (id : string) => {
      return await axios.get(ApiWrapper.backendURI + `chat?chatId=${id}`, {withCredentials: true});
    },

    decryptChat: async (chat: any[], rx_id: string) => {
      const key = await ApiWrapper.fetch_key(rx_id);
      chat.forEach((element) => {
        try {
          element.message = decryptMessage(element.message, key)   
        } catch (error) {
          console.log('Found Invalid Message')
        }
      });

      return chat;
    },

    fetch_key: async (rx_id : string) => {
      if (!(rx_id && rx_id in key_cache)){
        await requestKey(rx_id!)
        key_cache= (await getKeys())!.keys
      }
      return key_cache[rx_id]
    },
    
    sendMessage: async (formData : FormData) => {
      const rx_id = formData.get('rx_id')?.toString()
      let message = formData.get('message')
      let key = await ApiWrapper.fetch_key(rx_id!)

      message = encryptMessage(message, key)
      formData.set('message', message!.toString())
      console.log(decryptMessage(message!.toString(), key))

      return await axios.post(ApiWrapper.backendURI + 'chat/message', formData, {withCredentials: true});
    },
}
export default ApiWrapper;