import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import JoinUsContainer from '../components/JoinUsContainer';
import './Home.css';

interface Props {
  logged: boolean;
  uri: string;
}
const Home: React.FC<Props> = ({logged, uri}) => {
  return (
    <IonPage>
      <IonHeader>
        <IonToolbar>
          <IonTitle>Hello!</IonTitle>
        </IonToolbar>
      </IonHeader>
      <IonContent>
        <div>
          {logged ? (
          <ExploreContainer name="Homepage" />
          ) : (
          <JoinUsContainer uri={uri} />
        )}
        </div>
      </IonContent>
    </IonPage>
  );
};

export default Home;
