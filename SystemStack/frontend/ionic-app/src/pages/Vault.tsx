import { IonAlert, IonButton, IonChip, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonInput, IonItem, IonItemDivider, IonLabel, IonPage, IonPopover, IonRow, IonSearchbar, IonText, IonTitle, IonToolbar, useIonPopover } from '@ionic/react';
import { useContext, useEffect, useState } from 'react';
import ApiWrapper from '../support/APIWrapper';
import AppAppBar from '../components/AppAppBar';
import { addCircle, addOutline, cloudUpload, cloudUploadOutline, createOutline, trashBinOutline } from 'ionicons/icons';
import "../support/General.css"
import UploadComponent from '../components/UploadComponent';
import { UserContext } from '../App';
import { format } from 'date-fns';
const Vault: React.FC = () => {
  const [items, setItems] = useState<any[]>([]);
  const userDetails = useContext(UserContext);
  const [fileName, setFileName] = useState<string>("");
  useEffect(() => {
    fetchVault();
  }, [])

  const deleteItem = (id: string) => {
    const formData = new FormData;
    formData.append("id", id);
    ApiWrapper.deleteItem(formData)
    fetchVault()
  }

  const fetchVault = async() => {
    try{
      const response = await ApiWrapper.fetchVaultItems();
      if (response){
          setItems(response.data.items)
      }
      else{
        setItems([])
      }
    } catch (error){
    console.error('Error fetching Vault Details', error);
    }
  }

  const renameItem = async (id:string) => {
    try{
      
      const formData = new FormData;
      formData.append("id", id);
      formData.append("name", fileName);
      const response = await ApiWrapper.renameItem(formData);
      fetchVault()

      if (response){
          setFileName('')
      }
      else{
        setFileName('')
      }
    } catch (error){
    console.error('Error renaming file', error);
    }
  }

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Vault' />
      </IonHeader>
      <IonContent fullscreen className='ion-padding'>
        <IonRow>
          <IonCol>
              <IonSearchbar mode="ios" animated={true} color='' placeholder='Search for Items...'></IonSearchbar>
          </IonCol>
        </IonRow>
        <IonRow>
          <IonGrid className="ion-padding">
                <IonRow>
                  <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Size</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Type</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Creation</h2></IonLabel></IonCol>
                  <IonCol>
                    <IonButton color={'success'} size='small' fill='outline'  id="click-trigger">New<IonIcon icon={addOutline}/></IonButton></IonCol>
                <IonPopover trigger="click-trigger" triggerAction="click">
                  <UploadComponent vaultId={''} user={userDetails?.username} />
                </IonPopover>
                </IonRow>
                  {items.length !== 0 ? (
                    items.map((item) => (
                      <div key={item.id}>
                        <IonRow>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.name}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.size/1000/1024 > 1000 ? (item.size/1000/1024/1024).toFixed(2) + " GB": item.size/1000 > 1024 ? (item.size/1000/1024).toFixed(2) + " MB" : (item.size/1000).toFixed(2) + " KB"} </IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.type}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{format(item.createdAt, "dd-MM-yyyy HH:MM")}</IonLabel>
                          </IonCol>
                          <IonCol>
                            <IonButton id={"delete-" + item.id} shape='round' fill='outline' color={'danger'} size='small'><IonIcon size="medium" icon={trashBinOutline}/></IonButton>
                            <IonButton id={"rename-" + item.id} shape='round' fill='outline' color={'success'} size='small'><IonIcon size="medium" icon={createOutline}/></IonButton>
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
                              <IonInput placeholder='Insert file name' onIonChange={(e) => {if (e.detail.value)setFileName(e.detail.value)}}></IonInput>
                            </IonItem>
                            <IonItem>
                              <IonButton color={'success'} fill='outline' onClick={() => {if (fileName != '')  renameItem(item.id)}}>Rename</IonButton>
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
          </IonGrid>
        </IonRow>
      </IonContent>
    </IonPage>
  );
};

export default Vault;
