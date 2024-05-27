import { IonAlert, IonButton, IonCard, IonCardContent, IonChip, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInput, IonItem, IonItemDivider, IonLabel, IonPage, IonPopover, IonRow, IonSearchbar, IonText, IonTitle, IonToolbar, SearchbarInputEventDetail, useIonPopover } from '@ionic/react';
import { useContext, useEffect, useState } from 'react';
import ApiWrapper from '../support/APIWrapper';
import AppAppBar from '../components/AppAppBar';
import { addCircle, addOutline, cloudDownloadOutline, cloudUpload, cloudUploadOutline, createOutline, trashBinOutline } from 'ionicons/icons';
import "../support/General.css"
import UploadComponent from '../components/UploadComponent';
import { UserContext } from '../App';
import { format } from 'date-fns';
import { IonSearchbarCustomEvent } from '@ionic/core';
const Vault: React.FC = () => {
  const [items, setItems] = useState<any[]>([]);
  const userDetails = useContext(UserContext);
  const [fileName, setFileName] = useState<string>("");
  const [results, setResults] = useState<any[]>([])
  const [currentPage, setCurrentPage] = useState(1);
  const [itemsPerPage] = useState(10);
  useEffect(() => {
    fetchVault();
  }, [])

  const deleteItem = (id: string) => {
    const formData = new FormData;
    formData.append("id", id);
    ApiWrapper.deleteItem(formData)
    fetchVault()
  }

  const fetchVault = async () => {
    try {
      const response = await ApiWrapper.fetchVaultItems();
      if (response) {
        setItems(response.data.items)
        setResults(response.data.items)
      }
      else {
        setItems([])
      }
    } catch (error) {
      console.error('Error fetching Vault Details', error);
    }
  }

  const renameItem = async (id: string) => {
    try {

      const formData = new FormData;
      formData.append("id", id);
      formData.append("name", fileName);
      const response = await ApiWrapper.renameItem(formData);
      fetchVault()

      if (response) {
        setFileName('')
      }
      else {
        setFileName('')
      }
    } catch (error) {
      console.error('Error renaming file', error);
    }
  }

  const downloadFile = async (id: string, name: string, type: string,) => {
    try {
      const response = await ApiWrapper.downloadItem(id)
      const blob = new Blob([response!.data], { type: response!.headers['content-type'] });

      const url = window.URL.createObjectURL(blob);

      const link = document.createElement('a');
      link.href = url;
      link.setAttribute('download', `${name}.${type}`);
      document.body.appendChild(link);
      link.click();

      window.URL.revokeObjectURL(url);
      document.body.removeChild(link);

    } catch (error) {
      console.error('Error downloading file:', error);
    }
  }

  function handleItemInput(ev: IonSearchbarCustomEvent<SearchbarInputEventDetail>): void {
    const target = ev.target as HTMLIonSearchbarElement;
    const query = target ? target.value!.toLowerCase() : '';
    setResults(items.filter((d) => d.name.toLowerCase().indexOf(query) > -1));
    setCurrentPage(1);
  }

  const handlePageChange = (newPage: number) => {
    setCurrentPage(newPage);
  };

  const paginatedResults = results.slice((currentPage - 1) * itemsPerPage, currentPage * itemsPerPage);

  const renderPagination = () => {
    const totalPages = Math.ceil(results.length / itemsPerPage);
    const pages = [];
    for (let i = 1; i <= totalPages; i++) {
      pages.push(
        <a
          key={i}
          className={`${i === currentPage ? 'active' : ''}`}
          onClick={() => handlePageChange(i)}
          style={{
            border: '2px solid #007bff',
            borderRadius: '12px',
            padding: '5px 10px',
            marginRight: '5px',
            textDecoration: 'none',
            color: 'white',
            backgroundColor: i === currentPage ? '#007bff' : 'lightgray' // Adjust background color for active state
          }}
        >
          {i}
        </a>
      );
    }
    return pages;
  };
  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Vault' />
      </IonHeader>
      <IonContent fullscreen className='ion-padding'>
        <IonCard>
          <IonCardContent>
            <IonGrid>
              <IonRow>
                <IonCol>
                  <IonSearchbar mode="ios" animated={true} color='' placeholder='Search for a specific Item...' onIonInput={(ev) => handleItemInput(ev)}></IonSearchbar>
                </IonCol>
                <IonCol size='auto'>
                  <IonButton className="create-org" color={'success'} size='small' fill='outline' id="click-trigger">New<IonIcon icon={addOutline} /></IonButton>
                </IonCol>
                <IonPopover trigger="click-trigger" triggerAction="click">
                  <UploadComponent vaultId={''} user={userDetails?.username} />
                </IonPopover>
              </IonRow>
              <IonRow>
                <IonGrid className="ion-padding">
                  <IonRow>
                    <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                    <IonCol><IonLabel><h2>Size</h2></IonLabel></IonCol>
                    <IonCol><IonLabel><h2>Type</h2></IonLabel></IonCol>
                    <IonCol><IonLabel><h2>Creation</h2></IonLabel></IonCol>
                    <IonCol></IonCol>
                  </IonRow>
                  {paginatedResults.length !== 0 ? (
                    paginatedResults.map((item) => (
                      <div key={item.id}>
                        <IonRow>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.name}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.size / 1000 / 1024 > 1000 ? (item.size / 1000 / 1024 / 1024).toFixed(2) + " GB" : item.size / 1000 > 1024 ? (item.size / 1000 / 1024).toFixed(2) + " MB" : (item.size / 1000).toFixed(2) + " KB"} </IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.type}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{format(item.createdAt, "dd-MM-yyyy HH:MM")}</IonLabel>
                          </IonCol>
                          <IonCol>
                            <IonButton id={"delete-" + item.id} shape='round' fill='outline' color={'danger'} size='small'><IonIcon size="medium" icon={trashBinOutline} /></IonButton>
                            <IonButton id={"rename-" + item.id} shape='round' fill='outline' color={'success'} size='small'><IonIcon size="medium" icon={createOutline} /></IonButton>
                            <IonButton onClick={() => downloadFile(item.id, item.name, item.type)} id={"download-" + item.id} shape='round' fill='outline' color={'primary'} size='small'><IonIcon size="medium" icon={cloudDownloadOutline} /></IonButton>
                            <IonAlert
                              trigger={"delete-" + item.id}
                              trigger-action="click"
                              header="Delete File"
                              subHeader="This is irreversible!"
                              message="Do you wish to confirm file deletion?"
                              buttons={[
                                {
                                  text: 'YES',
                                  handler: () => {
                                    deleteItem(item.id);
                                  }
                                },
                                {
                                  text: 'NO',
                                  role: 'cancel',
                                  handler: () => {
                                  }
                                },
                              ]}
                            ></IonAlert>
                            <IonPopover
                              trigger={"rename-" + item.id}
                              trigger-action="click"

                            >
                              <IonItem>
                                <IonInput placeholder='Insert file name' onIonChange={(e) => { if (e.detail.value) setFileName(e.detail.value) }}></IonInput>
                              </IonItem>
                              <IonItem>
                                <IonButton color={'success'} fill='outline' onClick={() => { if (fileName != '') renameItem(item.id) }}>Rename</IonButton>
                              </IonItem>
                            </IonPopover>
                          </IonCol>
                        </IonRow>
                        <IonItemDivider>
                        </IonItemDivider>
                      </div>
                    ))
                  ) : (
                    <IonText>No Items stored yet. Start now! </IonText>
                  )}
                  <IonRow>
                    {results.length > 0 ? (
                      <div className="pagination ion-margin-top">
                        <a onClick={() => { currentPage > 1 ? setCurrentPage(currentPage - 1) : false }}>&laquo;</a>
                        {renderPagination()}
                        <a onClick={() => { currentPage < Math.ceil(results.length / itemsPerPage) ? setCurrentPage(currentPage + 1) : false }}>&raquo;</a>
                      </div>
                    ) : (<></>)}
                  </IonRow>
                </IonGrid>
              </IonRow>
            </IonGrid>
          </IonCardContent>
        </IonCard>
      </IonContent>
    </IonPage>
  );
};

export default Vault;
