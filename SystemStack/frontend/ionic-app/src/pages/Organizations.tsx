import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonChip, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonItem, IonItemDivider, IonLabel, IonList, IonMenu, IonPage, IonRouterOutlet, IonRow, IonSearchbar, IonSplitPane, IonTabs, IonText, IonTitle, IonToolbar } from '@ionic/react';
import { useEffect, useState } from 'react';
import "../support/General.css";
import { add, addCircle, atCircle, body, briefcase, checkmark, close, constructOutline } from 'ionicons/icons';
import CreateOrganizationContainer from '../components/CreateOrganizationContainer';
import ApiWrapper from '../support/APIWrapper';
import { Redirect, Route, useHistory } from 'react-router-dom';
import { IonReactRouter } from '@ionic/react-router';
import './Organizations.css';


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

const [selectedPage, setSelectedPage] = useState('organizations');

  const renderContent = () => {
    switch (selectedPage) {
      case 'organizations':
        return <OrganizationsView />;
      case 'invites':
        return <InvitesView />;
      default:
        return <OrganizationsView />;
    }
  };

  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Organizations</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent>
        <IonSplitPane when="xs" contentId="main">
          <IonMenu contentId="main">
            <IonContent>
              <IonList lines='full'>
                <IonItem button onClick={() => setSelectedPage('organizations')}>
                  <IonLabel><IonIcon icon={briefcase}></IonIcon>&nbsp;Organizations</IonLabel>
                </IonItem>
                <IonItem button onClick={() => setSelectedPage('invites')}>
                  <IonLabel><IonIcon icon={body}></IonIcon>&nbsp;Invites</IonLabel>
                </IonItem>
              </IonList>
            </IonContent>
          </IonMenu>

          <div className="ion-page" id="main">
            <IonContent className="ion-padding">
              {renderContent()}
            </IonContent>
          </div>
        </IonSplitPane>
      </IonContent>
    </IonPage>
  );
};

const OrganizationsView: React.FC = () => {
  const [organizations, setOrganizations] = useState<any[]>([]);
  const history = useHistory();

  useEffect(() => {
    fetchOrganizations();
  }, []);

  const refreshPage = async () => {
    fetchOrganizations();
  };

  const fetchOrganizations = async () => {
    try {
      const response = await ApiWrapper.fetchOrganizations();
      console.log(response?.data.organizations);
      setOrganizations(response ? response.data.organizations : []);
    } catch (error) {
      console.error('Error fetching User Organizations', error);
    }
  };

  const handleManageClick = (organizationId: string) => {
    history.push(`/organization/${organizationId}`);
  };

  return (
  <>
    <IonCard>
      <IonCardContent>You don't have any pending invite.</IonCardContent>
    </IonCard>
    <IonCard>
      <IonCardContent>
        <IonGrid>
          <IonRow>
            <IonCol>
              <IonSearchbar color='' placeholder='Search for organizations...'></IonSearchbar>
            </IonCol>
            <IonCol size='auto'>
              <IonButton className='create-org' color='success'>Add Organization</IonButton>
            </IonCol>
          </IonRow>
          <IonRow>
          <IonGrid>
                <IonRow>
                  <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Size</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Description</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Role</h2></IonLabel></IonCol>
                  <IonCol></IonCol>
                </IonRow>
                  {organizations.length !== 0 ? (
                    organizations.map((item) => (
                      <>
                        <IonRow key={item.organization.id}>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.organization.name}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.organization.size}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.organization.description}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            {item.roles.length !== 0 ? (
                              item.roles.map((item: any) => (
                                <IonChip outline={true}>{item.role}</IonChip>
                              ))) : (
                              <IonChip>No role</IonChip>
                            )}
                          </IonCol>
                          <IonCol>
                            <IonButton className='appt_button' size='small' shape='round' onClick={() => handleManageClick(item.organization.id)}>
                              Manage<IonIcon className="ion-padding-start" icon={constructOutline} />
                            </IonButton>
                          </IonCol>
                        </IonRow>
                        <IonItemDivider>
                        </IonItemDivider>
                      </>
                    ))
                  ) : (
                    <IonText>You don't belong to any organization yet.</IonText>
                  )}
          </IonGrid>
          </IonRow>
        </IonGrid>
      </IonCardContent>
    </IonCard>
  </>
  );
};

const InvitesView: React.FC = () => (
  <div>
    <h2>Invites</h2>
    <p>Invites View Content</p>
  </div>
);


export default Organizations;