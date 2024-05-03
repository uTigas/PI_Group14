import { IonContent, IonHeader, IonPage, IonTitle, IonToolbar } from '@ionic/react';

import { AuthContext, UserContext } from '../App';
import { useContext, useEffect } from 'react';

import AppAppBar from '../components/AppAppBar';
import JoinUsContainer from '../components/JoinUsContainer';

import './Home.css';

const Home: React.FC = () => {
  const logged = useContext(AuthContext);
  const user = useContext(UserContext);

  useEffect(() => {
    console.log("Home");
  }, []);

  return (
    <>
      {logged && user ? (
        <div className="container">
          <strong>Welcome {user.first_name} {user.last_name}</strong>
        </div>
      ) : (
        <JoinUsContainer />
      )}
    </>
  );
};

export default Home;
