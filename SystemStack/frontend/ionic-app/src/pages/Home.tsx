import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import JoinUsContainer from '../components/JoinUsContainer';
import './Home.css';
import { useContext } from 'react';
import { AuthContext, URIContext } from '../App';

const Home: React.FC = () => {
  const logged = useContext(AuthContext);
  const backendURI = useContext(URIContext);

  return (
    <IonPage>
      <IonContent>
        <div>
          {logged ? (
          <ExploreContainer name="Homepage" />
          ) : (
          <JoinUsContainer uri={backendURI} />
          )}
          </div>
      </IonContent>
    </IonPage>
  );
};

export default Home;
