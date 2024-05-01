import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonPage, IonRow, IonTitle, IonToolbar } from '@ionic/react';
import { useContext, useEffect } from 'react';
import { AuthContext, URIContext } from '../App';
import AccessDeniedContainer from '../components/AccessDeniedContainer';
import "./Organizations.css"
import "./General.css"
import { constructOutline, eye } from 'ionicons/icons';
import CreateOrganizationContainer from '../components/CreateOrganizationContainer';
import axios from 'axios';
const Organizations: React.FC = () => {

  const logged = useContext(AuthContext);
  const backendURI = useContext(URIContext);
  let ownedOrganizations: any[] = [];
  let memberOrganizations = [];
  const fetchOrganizations = async() => {
    try{
      const response = await axios.get(backendURI + 'organizations/member', {withCredentials: true});
      memberOrganizations = JSON.parse(response.data.data);
      const response2 = await axios.get(backendURI + 'organizations/owner', {withCredentials: true});
      ownedOrganizations = JSON.parse(response2.data.data);
      console.log(ownedOrganizations)
      console.log(memberOrganizations)

    } catch (error){
      console.error('Error fetching User Organizations', error);
    }
  }

  useEffect(() => {
    fetchOrganizations();
  })

  return (
    <IonPage>
      <IonContent className="ion-padding">
          {logged ? (
            <IonGrid className='grid'>
            <IonRow>
              <IonCol>
                <IonTitle className='ion-text-start ion-margin-bottom'>Create a Organization:</IonTitle>
                <IonCard>
                  <IonCardContent>
                    <CreateOrganizationContainer/>
                  </IonCardContent>
                </IonCard>
              </IonCol>
              <IonCol size-md="5" size="12" className="ion-text-center">
                <IonTitle className='ion-text-start ion-margin-bottom'>Your Organizations:</IonTitle>
                {ownedOrganizations.map((organization, index) => (
                  <IonCard className="card">
                    <IonCardHeader>
                      <IonCardTitle>{organization.fields.name}</IonCardTitle>
                      <IonCardSubtitle>2 members</IonCardSubtitle>
                    </IonCardHeader>
                    <IonCardContent>
                      <p>Sharing signage templates</p>
                      <IonButton size='small'>Manage<IonIcon className="ion-padding-start" icon={constructOutline}/></IonButton>
                    </IonCardContent>
                  </IonCard>
                ))}
              </IonCol>
            </IonRow>
          </IonGrid>         
          ):(
            <AccessDeniedContainer uri={backendURI}/>
          )}
      </IonContent>
    </IonPage>
  );
};

export default Organizations;
