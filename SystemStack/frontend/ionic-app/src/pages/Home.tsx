import { IonCard, IonCardContent, IonCol, IonContent, IonGrid, IonHeader, IonPage, IonRow, IonTitle } from '@ionic/react';
import { AuthContext, UserContext } from '../App';
import { useContext, useEffect, useState } from 'react';
import AppAppBar from '../components/AppAppBar';
import JoinUsContainer from '../components/JoinUsContainer';
import './Home.css';
import ApiWrapper from '../support/APIWrapper';

const Home: React.FC = () => {
  const logged = useContext(AuthContext);
  const user = useContext(UserContext);
  
  const [stats, setStats] = useState({
    organizations: 0,
    requests: 0,
    size: 0,
    users: 0,
    vaults: 0
  });

  const fetchStats = async () => {
    try {
      const response = await ApiWrapper.fetchStats();
      if (response && response.data) {
        setStats(response.data);
      }
    } catch (error) {
      console.error('Failed to fetch stats:', error);
    }
  };

  useEffect(() => {
    fetchStats();
  }, []);

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title='Home' />
      </IonHeader>
      <IonContent>
        {logged && user ? (
          <div className='containerHome'>
            <IonGrid>
              <IonRow>
                <IonCol></IonCol>
                <IonCol>
                  <IonCard>
                    <IonCardContent>
                      <div className='center-text'>
                        <IonTitle>Qeep Stats</IonTitle>
                      </div>
                      <p>Organizations: {stats.organizations}</p>
                      <p>Requests: {stats.requests}</p>
                      <p>Size: {stats.size}</p>
                      <p>Users: {stats.users}</p>
                      <p>Vaults: {stats.vaults}</p>
                    </IonCardContent>
                  </IonCard>
                </IonCol>
                <IonCol></IonCol>
              </IonRow>
            </IonGrid>
          </div>
        ) : (
          <JoinUsContainer />
        )}
      </IonContent>
    </IonPage>
  );
};

export default Home;
