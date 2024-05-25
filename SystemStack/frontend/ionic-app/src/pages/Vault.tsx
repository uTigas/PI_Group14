import { IonButton, IonChip, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonItemDivider, IonLabel, IonPage, IonRow, IonText, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import { useEffect, useState } from 'react';
import ApiWrapper from '../support/APIWrapper';

const Vault: React.FC = () => {
  const [items, setItems] = useState<any[]>([]);
  useEffect(() => {
    fetchVault();
  }, [])
  
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
  function handleManageClick(id: any): void {
    throw new Error('Function not implemented.');
  }

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Vault' />
      </IonHeader>
      <IonContent fullscreen>
        <IonRow>
          <IonGrid>
                <IonRow>
                  <IonCol><IonLabel><h2>Name</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Size</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Description</h2></IonLabel></IonCol>
                  <IonCol><IonLabel><h2>Role</h2></IonLabel></IonCol>
                  <IonCol></IonCol>
                </IonRow>
                  {items.length !== 0 ? (
                    items.map((item) => (
                      <>
                        <IonRow key={item.id}>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.name}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.size}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.type}</IonLabel>
                          </IonCol>
                          <IonCol className='appt_col'>
                            <IonLabel>{item.createdAt}</IonLabel>
                          </IonCol>
                        </IonRow>
                        <IonItemDivider>
                        </IonItemDivider>
                      </>
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
