import { IonButton, IonCard, IonCardContent, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonItem, IonItemDivider, IonLabel, IonList, IonMenu, IonPage, IonRow, IonSearchbar, IonSplitPane, IonTitle, IonToolbar, IonText, IonChip, IonModal, IonButtons, IonInput, IonTextarea, IonSelect, IonSelectOption } from '@ionic/react';
import { useEffect, useRef, useState } from 'react';
import "../support/General.css";
import { briefcase, mailOpen, constructOutline } from 'ionicons/icons';
import ApiWrapper from '../support/APIWrapper';
import { useHistory } from 'react-router-dom';
import './Organizations.css';
import { OverlayEventDetail } from '@ionic/react/dist/types/components/react-component-lib/interfaces';

const Organizations: React.FC = () => {
  const [selectedPage, setSelectedPage] = useState('organizations');

  const renderContent = () => {
    switch (selectedPage) {
      case 'organizations':
        return <OrganizationsView />;
      case 'invites':
        return <InvitesView />;
      case 'organization':
        return <OrganizationView />;
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
                  <IonLabel><IonIcon icon={mailOpen}></IonIcon>&nbsp;Invites</IonLabel>
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
  const [results, setResults] = useState<any[]>([]);
  const [currentPage, setCurrentPage] = useState(1);
  const [itemsPerPage] = useState(10);
  const history = useHistory();

  useEffect(() => {
    fetchOrganizations();
  }, []);

  const fetchOrganizations = async () => {
    try {
      const response = await ApiWrapper.fetchOrganizations();
      setOrganizations(response ? response.data.organizations : []);
      setResults(response ? response.data.organizations : []);
    } catch (error) {
      console.error('Error fetching User Organizations', error);
    }
  };

  const handleManageClick = (organizationId: string) => {
    history.push(`/organization/${organizationId}`);
  };

  const handleInput = (ev: Event) => {
    const target = ev.target as HTMLIonSearchbarElement;
    const query = target ? target.value!.toLowerCase() : '';
    setResults(organizations.filter((d) => d.organization.name.toLowerCase().indexOf(query) > -1));
    setCurrentPage(1);
  };

  const handlePageChange = (newPage: number) => {
    setCurrentPage(newPage);
  };

  const paginatedResults = results.slice((currentPage - 1) * itemsPerPage, currentPage * itemsPerPage);

  const renderPagination = () => {
    const totalPages = Math.ceil(results.length / itemsPerPage);
    const pages = [];
    for (let i = 1; i <= totalPages; i++) {
      pages.push(
        <a key={i} className={i == currentPage ? 'active' : ''} onClick={() => handlePageChange(i)}>{i}</a>
      );
    }
    return pages;
  };

  const modal = useRef<HTMLIonModalElement>(null);
  const input = useRef<HTMLIonInputElement>(null);

  const [name, setName] = useState('');
  const [description, setDescription] = useState('');
  const [size, setSize] = useState('');

  const [message, setMessage] = useState(
    'This modal example uses triggers to automatically open a modal when the button is clicked.'
  );

  function confirm() {
    if (validateForm()) {
      handleSubmit();
      modal.current?.dismiss(input.current?.value, 'confirm');
    }
  }

  function onWillDismiss(ev: CustomEvent<OverlayEventDetail>) {
    if (ev.detail.role === 'confirm') {
      setMessage(`Hello, ${ev.detail.data}!`);
    }
  }

  function handleSubmit() {
    const formData = new URLSearchParams();
    formData.append('name', name);
    formData.append('description', description);
    formData.append('size', size);
    ApiWrapper.createOrganization(formData);
    console.log('Submitted:', { name, description, size });
  }

  const validateForm = () => {
    return name.trim().length > 0  && size.trim().length > 0;
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
                <IonSearchbar mode='ios' animated={true} placeholder='Search for organizations...' onIonInput={(ev) => handleInput(ev)}></IonSearchbar>
              </IonCol>
              <IonCol size='auto'>
                <IonButton id='open-modal' className='create-org' fill='outline' shape='round' color='success'>Add Organization</IonButton>
              </IonCol>
            </IonRow>
            <IonRow>
              <IonGrid>
                <IonRow>
                  <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Size</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Description</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Roles</h2></IonLabel></IonCol>
                  <IonCol></IonCol>
                </IonRow>
                <IonItemDivider></IonItemDivider>
                {paginatedResults.length !== 0 ? (
                  paginatedResults.map((item) => (
                    <div key={item.organization.id}>
                      <IonRow>
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
                            item.roles.map((role: any, idx: any) => (
                              <IonChip key={idx} color='success' outline={true}>{role.role}</IonChip>
                            ))) : (
                            <IonChip>No role</IonChip>
                          )}
                        </IonCol>
                        <IonCol>
                          <IonButton className='appt_button' fill='outline' shape='round' href={'/organization/' + item.organization.id}>
                            Manage<IonIcon className="ion-padding-start" icon={constructOutline} />
                          </IonButton>
                        </IonCol>
                      </IonRow>
                      <IonItemDivider></IonItemDivider>
                    </div>
                  ))
                ) : (
                  <IonText>No organizations found.</IonText>
                )}
              </IonGrid>
            </IonRow>
            <IonRow>
              <div className="pagination">
                  <a onClick={() => {currentPage > 1 ? setCurrentPage(currentPage-1) : false}}>&laquo;</a>
                        {renderPagination()}
                  <a onClick={() => {currentPage < Math.ceil(results.length / itemsPerPage) ? setCurrentPage(currentPage+1) : false}}>&raquo;</a>
              </div>
            </IonRow>
          </IonGrid>
          <IonModal ref={modal} trigger="open-modal" onWillDismiss={(ev) => onWillDismiss(ev)}>
            <IonHeader>
              <IonToolbar mode='ios'>
                <IonButtons slot="start">
                  <IonButton onClick={() => modal.current?.dismiss()}>Cancel</IonButton>
                </IonButtons>
                <IonTitle>Create Organization</IonTitle>
                <IonButtons slot="end">
                  <IonButton strong={true} onClick={() => confirm()}>
                    Confirm
                  </IonButton>
                </IonButtons>
              </IonToolbar>
            </IonHeader>
            <IonContent className="ion-padding">
              <IonItem>
                <IonInput
                  label='Organization&#39;s name'
                  labelPlacement="stacked"
                  type="text"
                  value={name}
                  onIonChange={(e) => {
                    setName(e.detail.value!);
                    validateForm();
                  }}
                />
              </IonItem>

              <IonItem>
                <IonTextarea
                  label='Short description'
                  value={description}
                  labelPlacement="stacked"
                  onIonChange={(e) => {
                    setDescription(e.detail.value!);
                    validateForm();
                  }}
                />
              </IonItem>

              <IonItem>
                <IonSelect
                  label='Organization&#39;s size'
                  value={size}
                  labelPlacement="stacked"
                  onIonChange={(e) => {
                    setSize(e.detail.value!);
                    validateForm();
                  }}
                  placeholder="Select Size"
                >
                  <IonSelectOption value="small">Small</IonSelectOption>
                  <IonSelectOption value="medium">Medium</IonSelectOption>
                  <IonSelectOption value="large">Large</IonSelectOption>
                </IonSelect>
              </IonItem>
            </IonContent>
          </IonModal>
        </IonCardContent>
      </IonCard>
    </>
  );
};

const OrganizationView: React.FC = () => (
  <div>
    <h2>Organization View</h2>
    <p>View Content</p>
  </div>
);

const InvitesView: React.FC = () => (
  <div>
    <h2>Invites</h2>
    <p>Invites View Content</p>
  </div>
);

export default Organizations;
