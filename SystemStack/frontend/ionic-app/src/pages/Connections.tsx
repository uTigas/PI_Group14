import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import './Connections.css';
import InfiniteContainer from '../components/InfiniteContainer';

const Connections: React.FC = () => {
  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Connections</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent fullscreen>
        <IonHeader collapse="condense">   
        </IonHeader>
        <InfiniteContainer items={["Olá","Olá2","Olá3","Olá4","Olá5","Olá6","Olá7","Olá8"]} numItems={4} />
      </IonContent>
    </IonPage>
  );
};

export default Connections;
