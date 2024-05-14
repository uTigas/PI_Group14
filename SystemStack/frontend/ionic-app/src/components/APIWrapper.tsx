import axios from "axios";


const ApiWrapper = {
    backendURI : 'http://localhost:8000/',

    checkAuthentication : async () => {
        try {
          return await axios.get(ApiWrapper.backendURI + 'check-authentication', {withCredentials: true});
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
          return await axios.get(ApiWrapper.backendURI + 'organizations/details?id=' + organizationId, {withCredentials: true});
        } catch (error){
          console.error('Error fetching Organization details', error);
        }
    },

    createVault: async (formData : URLSearchParams) => {
      await axios.post(ApiWrapper.backendURI + "organizations/details/vaults/create", formData, {
        withCredentials: true, 
        headers: {
        'Content-Type': 'application/x-www-form-urlencoded'
        }
      })
      .then(response => console.log(response))
      .catch(error => console.log(error))
    }

}
export default ApiWrapper;
