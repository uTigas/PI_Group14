import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInfiniteScroll, IonInfiniteScrollContent, IonPage, IonRouterOutlet, IonRow, IonTitle, IonToolbar } from '@ionic/react';
import { useEffect, useState} from 'react';
import "./Organizations.css"
import "./General.css"
import { checkmark, close, constructOutline, eye } from 'ionicons/icons';
import CreateOrganizationContainer from '../components/CreateOrganizationContainer';
import ApiWrapper from '../components/APIWrapper';
import { Route, useHistory } from 'react-router-dom';
const Organizations: React.FC = () => {
  const [organizations, setOrganizations] = useState<any[]>([]);
  const history = useHistory();

  useEffect(() => {
    fetchOrganizations();
  }, [])

  const fetchOrganizations = async() => {
    try{
      const response = await ApiWrapper.fetchOrganizations();
      if (response){
        setOrganizations(response.data.organizations);
      }
      else{
        setOrganizations([])
      }
    } catch (error){
      console.error('Error fetching User Organizations', error);
    }
  }

  const handleManageClick = (organizationId: string) => {
    history.push(`/organization/${organizationId}`);
  };
  return (
    <IonPage>
      <IonContent className="ion-padding">
            <IonGrid className='grid'>
            <IonRow>
              <IonCol size-md='5' size='12'>
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
              <IonCol className="ion-text-center">
                <IonTitle className='ion-text-start ion-margin-bottom title'>Your Organizations:</IonTitle>
                {organizations.length !== 0 ? (
                    organizations.map((item, index) => (
                      <IonCard className="card  organization-card" key={item.organization.id}>
                        <IonCardHeader>
                          <IonCardTitle>{item.organization.name}</IonCardTitle>
                          <IonCardSubtitle>2 members</IonCardSubtitle>
                        </IonCardHeader>
                        <IonCardContent>
                          <p>{item.organization.description}</p>
                          <IonButton size='small' shape='round' onClick={() => handleManageClick(item.organization.id)}>Manage<IonIcon className="ion-padding-start" icon={constructOutline}/></IonButton>
                        </IonCardContent>
                      </IonCard>
                    ))
                ):(
                    <p>No organizations found.</p>
                )} 
              </IonCol>
            </IonRow>
          </IonGrid>
      </IonContent>
    </IonPage>
  );
};

export default Organizations;
