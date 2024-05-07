import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInfiniteScroll, IonInfiniteScrollContent, IonPage, IonRow, IonTitle, IonToolbar } from '@ionic/react';
import { useContext, useEffect, useState} from 'react';
import { AuthContext, URIContext } from '../App';
import AccessDeniedContainer from '../components/AccessDeniedContainer';
import "./Organizations.css"
import "./General.css"
import { checkmark, close, constructOutline, eye } from 'ionicons/icons';
import CreateOrganizationContainer from '../components/CreateOrganizationContainer';
import axios from 'axios';
const Organizations: React.FC = () => {
  const logged = useContext(AuthContext);
  const backendURI = useContext(URIContext);
  const [ownedOrganizations, setOwned] = useState<any[]>([]);
  const [memberOrganizations, setMember] = useState<any[]>([]);
  
  useEffect(() => {
    fetchOrganizations();
  }, [])

  const fetchOrganizations = async() => {
    try{
      const response = await axios.get(backendURI + 'organizations/member', {withCredentials: true});
      setMember(JSON.parse(response.data.organizations));
      const response2 = await axios.get(backendURI + 'organizations/owner', {withCredentials: true});
      setOwned(JSON.parse(response2.data.organizations));

    } catch (error){
      console.error('Error fetching User Organizations', error);
    }
  }

  return (
    <IonPage>
      <IonContent className="ion-padding">
          {logged ? (
            <IonGrid className='grid'>
            <IonRow>
              <IonCol size-md='4' size='12'>
                <IonTitle className='ion-text-start ion-margin-bottom title'>Pending Invites:</IonTitle>
                  <IonCard>
                    <IonCardContent>
                      <strong style={{'color':'blue'}}>Gregório</strong><strong> invited you to join <strong style={{'color':'yellow'}}>PQSK</strong></strong>
                      <IonButton slot='icon' className='ion-margin-start' color='success'><IonIcon icon={checkmark} size='small'></IonIcon></IonButton>
                      <IonButton slot='icon' className='ion-margin-start' color='danger'><IonIcon icon={close} size='small'></IonIcon></IonButton>

                    </IonCardContent>
                </IonCard>
                <IonTitle className='ion-text-start ion-margin-vertical title'>Create a Organization:</IonTitle>
                <IonCard>
                  <IonCardContent>
                    <CreateOrganizationContainer/>
                  </IonCardContent>
                </IonCard>
              </IonCol>
              <IonCol size-md="4" size="12" className="ion-text-center">
                <IonTitle className='ion-text-start ion-margin-bottom title'>Your Organizations:</IonTitle>
                {ownedOrganizations.map((organization, index) => (
                  <IonCard className="card  organization-card" key={organization.pk}>
                    <IonCardHeader>
                      <IonCardTitle>{organization.fields.name}</IonCardTitle>
                      <IonCardSubtitle>2 members</IonCardSubtitle>
                    </IonCardHeader>
                    <IonCardContent>
                      <p>{organization.fields.description}</p>
                      <IonButton size='small' shape='round'>Manage<IonIcon className="ion-padding-start" icon={constructOutline}/></IonButton>
                    </IonCardContent>
                  </IonCard>
                ))}
              </IonCol>
              <IonCol size-md="4" size="12" className="ion-text-center">
                <IonTitle className='ion-text-start ion-margin-bottom ion-margin-top title'>Member Organizations:</IonTitle>
                <IonInfiniteScroll>
                  {memberOrganizations.map((organization, index) => (
                      <IonCard className="card organization-card" key={organization.pk}>
                        <IonCardHeader>
                          <IonCardTitle>{organization.fields.name}</IonCardTitle>
                          <IonCardSubtitle>2 members</IonCardSubtitle>
                        </IonCardHeader>
                        <IonCardContent>
                          <p>{organization.fields.description}</p>
                          <IonButton size='small' shape='round'>Manage<IonIcon className="ion-padding-start" icon={constructOutline}/></IonButton>
                        </IonCardContent>
                      </IonCard>
                    ))}
                </IonInfiniteScroll>
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
