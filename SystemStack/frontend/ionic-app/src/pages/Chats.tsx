import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import AppAppBar from '../components/AppAppBar';

const Chats: React.FC = () => {
  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Chats' />
      </IonHeader>
      <IonContent></IonContent>
    </IonPage>
  );
};

export default Chats;
