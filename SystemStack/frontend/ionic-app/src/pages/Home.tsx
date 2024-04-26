import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/react';
import ExploreContainer from '../components/ExploreContainer';
import JoinUsContainer from '../components/JoinUsContainer';
import './Home.css';
import { useContext } from 'react';
import { AuthContext, URIContext } from '../App';
import { UserContext } from '../App';
const Home: React.FC = () => {
  const logged = useContext(AuthContext);
  const user = useContext(UserContext);

  return (
    <IonPage>
      <IonContent>
          {logged && user ? (
          
          ) : (
          <JoinUsContainer />
          )}
      </IonContent>
    </IonPage>
  );
};

export default Home;
