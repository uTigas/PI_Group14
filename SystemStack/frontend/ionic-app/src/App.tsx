import { Redirect, Route } from 'react-router-dom';
import {
  IonApp,
  IonButton,
  IonCol,
  IonGrid,
  IonHeader,
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
import { ellipse, help, home, lockClosed, mail, people, person, square, triangle } from 'ionicons/icons';
import Tab1 from './pages/Tab1';
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
import Organizations from './pages/Organizations';
import { useContext, useEffect, useState } from 'react';
import axios from 'axios';
import AboutUs from './pages/AboutUs';
import Dashboard from './pages/Dashboard';
import React from 'react';

setupIonicReact();
const backendURI="http://localhost:8000/"
export const AuthContext = React.createContext(false);
export const URIContext = React.createContext("http://localhost:8000/");

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
    <AuthContext.Provider value={loggedIn}>
        <IonApp>
          <IonHeader>
            <IonToolbar>
              <IonGrid>
                <IonRow className="ion-align-items-center">
                  <IonCol size='auto' className="ion-text-center">
                    <IonTitle>Vault</IonTitle>
                  </IonCol>
                  {loggedIn ? (
                    <IonCol className="ion-text-end">
                      <IonButton href={useContext(URIContext) + "logout"} color="danger">Logout</IonButton>
                    </IonCol>
                  ) : (
                    <IonCol className="ion-text-end">
                      <IonButton href={useContext(URIContext) + "login"} color="success">Login</IonButton>
                    </IonCol>
                  )}
                </IonRow>
              </IonGrid>
            </IonToolbar>
          </IonHeader>
          <IonReactRouter>
            <IonTabs>
              <IonRouterOutlet>
                <Route exact path="/home">
                  <Home />
                </Route>
                <Route exact path="/organizations">
                  <Organizations />
                </Route>
                <Route path="/chats">
                  <Chats />
                </Route>
                <Route exact path="/aboutUs">
                  <AboutUs />
                </Route>
                <Route exact path="/dashboard">
                  <Dashboard />
                </Route>
                <Route exact path="/">
                  <Redirect to='/home'></Redirect>
                </Route>
              </IonRouterOutlet>
              {loggedIn ?(
                <IonTabBar slot="bottom">
                <IonTabButton tab="home" href="/home" >
                    <IonIcon aria-hidden="true" icon={home} />
                    <IonLabel>Homepage</IonLabel>
                  </IonTabButton>
                  <IonTabButton tab="organizations" href="/organizations">
                    <IonIcon aria-hidden="true" icon={people} />
                    <IonLabel>Organizations</IonLabel>
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
              ):(
                <IonTabBar slot="bottom">
                <IonTabButton tab="home" href="/home">
                    <IonIcon aria-hidden="true" icon={home} />
                    <IonLabel>Homepage</IonLabel>
                  </IonTabButton>
                  <IonTabButton tab="aboutUs" href="/aboutUs">
                    <IonIcon aria-hidden="true" icon={help} />
                    <IonLabel>AboutUs</IonLabel>
                  </IonTabButton>
                </IonTabBar>
              )}
            </IonTabs>
          </IonReactRouter>
        </IonApp>
    </AuthContext.Provider>
  );
};

export default App;
