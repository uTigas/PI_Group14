import {
  IonCard,
  IonCardContent,
  IonCol,
  IonContent,
  IonGrid,
  IonHeader,
  IonImg,
  IonItemDivider,
  IonPage,
  IonRow,
  IonText,
  IonTitle
} from '@ionic/react';
import { AuthContext, UserContext } from '../App';
import { useContext, useEffect, useState } from 'react';
import AppAppBar from '../components/AppAppBar';
import JoinUsContainer from '../components/JoinUsContainer';
import ApiWrapper from '../support/APIWrapper';
import background from '../../resources/background.png';

import shield from '../../resources/security-shield.png'

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

  const styles = {
    page: {
      height: '100%',
      display: 'flex',
      flexDirection: 'column',
    },
    containerHome: {
      width: '100%',
      maxWidth: '1200px',
      margin: 'auto'
    },
    statsCard: {
      borderRadius: '10px',
      boxShadow: '0 4px 8px rgba(0, 0, 0, 0.1)',
      marginTop: '20px',
    },
    centerText: {
      marginBottom: '20px'
    },
    statText: {
      fontSize: '18px',
      margin: '10px 0',
      fontWeight: 500
    },
    statLabel: {
      color: '#007bff'
    },
    introCard: {
      boxShadow: '0 0 0 rgba(0, 0, 0, 0)',
      marginTop: '40px',
      marginLeft: '40px',
      width: '500px'
    },
    headerText: {
      color: '#333',
      fontSize: '24px',
      fontWeight: 'bold',
      marginBottom: '10px'
    },
    descriptionText: {
      color: '#555',
      fontSize: '16px',
      marginBottom: '3px',
      marginTop: '3px'
    },
    highlightText: {
      color: '#007bff',
      fontWeight: 'bold'
    }
  };

  return (
    <IonPage>
      <IonHeader>
        <AppAppBar title="Home" />
      </IonHeader>
      <IonContent>
        <div style={styles.page}>
        {logged && user ? (
          <div style={styles.containerHome}>
            <IonGrid>
              <IonRow>
                <IonCol size='auto'>
                  <div>
                  <IonCard style={styles.introCard}>
                    <IonCardContent>
                      <div style={styles.centerText}>
                        <IonTitle style={styles.headerText}>Welcome to Qeep</IonTitle>
                        <p style={styles.descriptionText}>
                          Qeep is your secure solution for managing sensitive files and information. 
                          Whether you're an organization or an individual, we provide a safe and 
                          user-friendly platform to store and access your secrets.
                        </p>
                        <IonItemDivider/>
                        <br/>
                        <p style={styles.descriptionText}>
                          <IonText color='primary'>Organizations:</IonText> Create vaults, manage access, and ensure 
                          your data is protected.
                        </p>
                        <p style={styles.descriptionText}>
                          <IonText color='primary'>Individuals:</IonText> Keep your personal documents and files safe 
                          and accessible only to you.
                        </p>
                      </div>
                    </IonCardContent>
                  </IonCard>
                  </div>
                </IonCol>
                <IonCol >
                    <IonImg src={shield} alt="Logo"  style={{width: '350px', float: 'right', 'marginRight': '30px'}}/> </IonCol>
                </IonRow>
                        <IonItemDivider/>
                <IonRow>
                <IonCol>
                  <div>
                      <IonCard style={styles.statsCard}>
                        <IonCardContent>
                          <div style={styles.centerText}>
                            <IonTitle>System  Statistics</IonTitle>
                          </div>
                          <p style={styles.descriptionText}>
                            <IonText >Organizations:</IonText> {stats.organizations}
                          </p>
                          <IonItemDivider />
                          <p style={styles.descriptionText}>
                            <IonText >Size:</IonText> {stats.size}
                          </p>
                          <IonItemDivider />
                          <p style={styles.descriptionText}>
                            <IonText >Users:</IonText> {stats.users}
                          </p>
                          <IonItemDivider />
                          <p style={styles.descriptionText}>
                            <IonText >Vaults:</IonText> {stats.vaults}
                          </p>
                        </IonCardContent>
                      </IonCard>
                  </div>
                </IonCol>
              </IonRow>
            </IonGrid>
          </div>
        ) : (
          <JoinUsContainer />
        )}

        </div>
      </IonContent>
    </IonPage>
  );
};

export default Home;
