import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import InfiniteContainer from '../components/InfiniteContainer';

const AboutUs: React.FC = () => {
  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>AboutUs</IonTitle>
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

export default AboutUs;
