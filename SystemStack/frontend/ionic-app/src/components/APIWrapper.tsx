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
          let memberOrgs = []
          let ownedOrgs = []
          const response = await axios.get(ApiWrapper.backendURI + 'organizations/member', {withCredentials: true});
          memberOrgs = JSON.parse(response.data.organizations);
          const response2 = await axios.get(ApiWrapper.backendURI + 'organizations/owner', {withCredentials: true});
          ownedOrgs = JSON.parse(response2.data.organizations);
          return [memberOrgs, ownedOrgs];
        } catch (error){
          console.error('Error fetching User Organizations', error);
        }
      }
}
export default ApiWrapper;
