import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import AppAppBar from '../components/AppAppBar';

const Vault: React.FC = () => {
  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Vault' />
      </IonHeader>
      <IonContent fullscreen>
        <ExploreContainer name="Homepage" />
      </IonContent>
    </IonPage>
  );
};

export default Vault;
