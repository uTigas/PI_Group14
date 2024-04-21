import { Redirect, Route } from 'react-router-dom';
import {
  IonApp,
  IonButton,
  IonCol,
  IonGrid,
  IonIcon,
  IonLabel,
  IonRouterOutlet,
  IonRow,
  IonTabBar,
  IonTabButton,
  IonTabs,
  IonText,
  IonTitle,
  IonToolbar,
  setupIonicReact
} from '@ionic/react';
import { IonReactRouter } from '@ionic/react-router';
import { ellipse, home, lockClosed, mail, people, person, square, triangle } from 'ionicons/icons';
import Tab1 from './pages/Tab1';
import Tab2 from './pages/Tab2';
import Chats from './pages/Chats';

/* Core CSS required for Ionic components to work properly */
import '@ionic/react/css/core.css';

/* Basic CSS for apps built with Ionic */
import '@ionic/react/css/normalize.css';
import '@ionic/react/css/structure.css';
import '@ionic/react/css/typography.css';

/* Optional CSS utils that can be commented out */
import '@ionic/react/css/padding.css';
import '@ionic/react/css/float-elements.css';
import '@ionic/react/css/text-alignment.css';
import '@ionic/react/css/text-transformation.css';
import '@ionic/react/css/flex-utils.css';
import '@ionic/react/css/display.css';

/* Theme variables */
import './theme/variables.css';
import Home from './pages/Home';
import Connections from './pages/Connections';
import { useEffect, useState } from 'react';
import axios from 'axios';

setupIonicReact();
const backendURI="http://localhost:8000"

const App: React.FC = () => {
  const [loggedIn, setLoggedIn] = useState(false);

  useEffect(() => {
    const checkAuthentication = async () => {
      console.log("Checking authentication...")
      try {
        const response = await axios.get('http://localhost:8000/check-authentication', {withCredentials: true});
        console.log("Is user authenticated? " + response.data.is_authenticated)
        setLoggedIn(response.data.is_authenticated);
      } catch (error) {
        console.error('Error checking authentication:', error);
      }
    };

    checkAuthentication();
  }, []); 

  return (
  <IonApp>
    <IonToolbar>
      <IonGrid>
        <IonRow>
          <IonCol size='auto'>
            <IonTitle>Vault</IonTitle>
            {loggedIn ? (
              <IonCol size='auto'>
                <IonButton fill='clear'>Logout</IonButton>
              </IonCol>
            ) : (
              <IonCol size='auto'>
                <IonButton href='/login' fill='clear'>Login</IonButton>
              </IonCol>
            )}
          </IonCol>
        </IonRow>
      </IonGrid>
    </IonToolbar>

    <IonReactRouter>
      <IonTabs>
        <IonRouterOutlet>
          <Route exact path="/tab1">
            <Tab1 />
          </Route>
          <Route exact path="/connections">
            <Connections />
          </Route>
          <Route path="/chats">
            <Chats />
          </Route>
          <Route exact path="/">
            <Home />
          </Route>
        </IonRouterOutlet>
        
        <IonTabBar slot="bottom">
        <IonTabButton tab="home" href="/home">
            <IonIcon aria-hidden="true" icon={home} />
            <IonLabel>Homepage</IonLabel>
          </IonTabButton>
          <IonTabButton tab="connections" href="/connections">
            <IonIcon aria-hidden="true" icon={people} />
            <IonLabel>Connections</IonLabel>
          </IonTabButton>
          <IonTabButton tab="tab2" href="/tab2">
            <IonIcon aria-hidden="true" icon={lockClosed} />
            <IonLabel>Vault</IonLabel>
          </IonTabButton>
          <IonTabButton tab="chats" href="/chats">
            <IonIcon aria-hidden="true" icon={mail} />
            <IonLabel>Chats</IonLabel>
          </IonTabButton>
        </IonTabBar>
      </IonTabs>
    </IonReactRouter>
  </IonApp>
  );
};

export default App;
