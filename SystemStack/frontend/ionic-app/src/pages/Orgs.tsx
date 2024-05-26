import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonItem, IonPage, IonRow, IonText, IonTitle, IonToolbar } from '@ionic/react';
import { useEffect, useState } from 'react';
import "../support/General.css";
import { checkmark, close, constructOutline } from 'ionicons/icons';
import CreateOrganizationContainer from '../components/CreateOrganizationContainer';
import ApiWrapper from '../support/APIWrapper';
import { useHistory } from 'react-router-dom';
import { format } from 'date-fns';


const Organizations: React.FC = () => {
  const [organizations, setOrganizations] = useState<any[]>([]);
  const history = useHistory();
  const [invites, setInvites] = useState<any[]>([]);
  
  useEffect(() => {
    fetchOrganizations();
    fetchInvites();
  }, []);

  const refreshPage = async () => {
    fetchOrganizations();
    fetchInvites();
  };

  const fetchOrganizations = async () => {
    try {
      const response = await ApiWrapper.fetchOrganizations();
      setOrganizations(response ? response.data.organizations : []);
    } catch (error) {
      console.error('Error fetching User Organizations', error);
    }
  };

  const fetchInvites = async () => {
    try {
      const response = await ApiWrapper.fetchInvites();
      setInvites(response ? response.data.invites : []);
    } catch (error) {
      console.error('Error fetching User Invites', error);
    }
  };

  const handleManageClick = (organizationId: string) => {
    history.push(`/organization/${organizationId}`);
  };

  const acceptInvite = async (id: string) => {
    const formData = new URLSearchParams();
    formData.append("invite", id);
    await ApiWrapper.acceptInvite(formData);
    refreshPage();
  };

  const refuseInvite = async (id: string) => {
    const formData = new URLSearchParams();
    formData.append("invite", id);
    await ApiWrapper.refuseInvite(formData);
    refreshPage();
  };

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Organizations</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent className="ion-padding">
        <IonGrid className='grid'>
          <IonRow>
            <IonCol sizeMd='4' size='12' className='dashboard-col'>
              <IonCard>
                <IonCardHeader>
                  <img alt="Silhouette of mountains" src="resources/images/hand-shake.png" className="section-image"/>
                  <IonCardTitle>Pending Invites</IonCardTitle>
                </IonCardHeader>
                <IonCardContent>
                  {invites.length === 0 ? (
                    <IonText>No invites available.</IonText>
                  ) : (
                    invites.map((invite) => (
                      <IonCard key={invite.id} className='invite-card'>
                        <IonCardContent>
                          <IonText style={{ 'color': 'lightgreen' }}>{invite.inviter}</IonText> invited you to join <IonText style={{ 'color': 'yellow' }}>{invite.organization}</IonText>
                          <IonButton slot='icon-only' color='success' onClick={() => acceptInvite(invite.id)}>
                            <IonIcon icon={checkmark} />
                          </IonButton>
                          <IonButton slot='icon-only' color='danger' onClick={() => refuseInvite(invite.id)}>
                            <IonIcon icon={close} />
                          </IonButton>
                          <IonText className='detail-text'>{format(new Date(invite.timestamp), "yyyy-MM-dd HH:mm:ss")}</IonText>
                        </IonCardContent>
                      </IonCard>
                    ))
                  )}
                </IonCardContent>
              </IonCard>
              <IonCard>
                <IonCardHeader>
                  <img alt="Silhouette of mountains" src="resources/images/building-construction.png" className="section-image"/>
                  <IonCardTitle>Create an Organization</IonCardTitle>
                </IonCardHeader>
                <IonCardContent>
                  <CreateOrganizationContainer />
                </IonCardContent>
              </IonCard>
            </IonCol>
            <IonCol className='dashboard-content'>
              <IonCard>
                <IonCardHeader>
                  <IonCardTitle>Your Organizations</IonCardTitle>
                </IonCardHeader>
                <IonCardContent>
                  {organizations.length !== 0 ? (
                    organizations.map((item) => (
                      <IonCard key={item.organization.id} className="organization-card">
                        <IonCardHeader>
                          <IonCardTitle>{item.organization.name}</IonCardTitle>
                          <IonCardSubtitle>{item.membersCount} members</IonCardSubtitle>
                        </IonCardHeader>
                        <IonCardContent>
                          <p>{item.organization.description}</p>
                          <IonButton size='small' shape='round' onClick={() => handleManageClick(item.organization.id)}>
                            Manage<IonIcon className="ion-padding-start" icon={constructOutline} />
                          </IonButton>
                        </IonCardContent>
                      </IonCard>
                    ))
                  ) : (
                    <IonText>No organizations found.</IonText>
                  )}
                </IonCardContent>
              </IonCard>
            </IonCol>
          </IonRow>
        </IonGrid>
      </IonContent>
    </IonPage>
  );
};

export default Organizations;