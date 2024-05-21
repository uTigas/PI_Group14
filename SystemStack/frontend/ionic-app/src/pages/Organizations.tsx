import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInfiniteScroll, IonInfiniteScrollContent, IonItem, IonPage, IonRouterOutlet, IonRow, IonText, IonTitle, IonToolbar } from '@ionic/react';
import { useEffect, useState} from 'react';
import "./Organizations.css"
import "./General.css"
import { checkmark, close, constructOutline, eye } from 'ionicons/icons';
import CreateOrganizationContainer from '../components/CreateOrganizationContainer';
import ApiWrapper from '../support/APIWrapper';
import { Route, useHistory } from 'react-router-dom';
import { format } from 'date-fns';

const Organizations: React.FC = () => {
  const [organizations, setOrganizations] = useState<any[]>([]);
  const history = useHistory();
  const [invites, setInvites] = useState<any[]>([])
  useEffect(() => {
    fetchOrganizations();
    fetchInvites();
  }, [])

  const refreshPage = async() => {
    fetchOrganizations();
    fetchInvites();
  }
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

  const fetchInvites = async() => {
    const response = await ApiWrapper.fetchInvites();
    try {
      if (response){
        setInvites(response.data.invites);
      }
      else{
        setInvites([])
      }
    } catch (error) {
      console.error('Error fetching User Invites', error);
    }
  }

  const handleManageClick = (organizationId: string) => {
    history.push(`/organization/${organizationId}`);
  };

  const acceptInvite = async (id : string) => {
    const formData = new URLSearchParams();
    formData.append("invite", id);
    await ApiWrapper.acceptInvite(formData);
    refreshPage();
  }

  const refuseInvite = async (id : string) => {
    const formData = new URLSearchParams();
    formData.append("invite", id);
    await ApiWrapper.refuseInvite(formData);
    refreshPage();
  }

  return (
    <IonPage>
      <IonContent className="ion-padding">
            <IonGrid className='grid'>
            <IonRow>
              <IonCol size-md='5' size='12'>
                <IonTitle className='ion-text-start ion-margin-bottom title'>Pending Invites:</IonTitle>
                {invites.length == 0 ?(<IonText className='ion-margin-start' color={"warning"}>No pending invites.</IonText>):(<></>)}
                {invites.map((invite) => (
                  <IonCard key={invite.id}>
                    <IonCardContent>
                      <IonText style={{'color':'lightgreen'}}>{invite.inviter}</IonText><IonText> invited you to join <IonText style={{'color':'yellow'}}>{invite.organization}</IonText></IonText>
                      <IonButton slot='icon' className='ion-margin-start' color='success' onClick={() => acceptInvite(invite.id)}><IonIcon icon={checkmark} size='small'></IonIcon></IonButton>
                      <IonButton slot='icon' className='ion-margin-start' color='danger' onClick={() => refuseInvite(invite.id)}><IonIcon icon={close} size='small'></IonIcon></IonButton>
                      <IonItem>
                        <IonCardSubtitle>{format(invite.timestamp, "yyyy-MM-dd HH:mm:ss")}</IonCardSubtitle>
                      </IonItem>
                    </IonCardContent>
                </IonCard>
                ))}
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
