import { Redirect, Route } from 'react-router-dom';
import { useContext, useEffect, useState } from 'react';
import { IonApp, IonButton, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonItem, IonLabel, IonList, IonPage, IonPopover, IonRouterOutlet, IonRow, IonTabBar, IonTabButton, IonTabs, IonToolbar, setupIonicReact } from '@ionic/react';
import { IonReactRouter } from '@ionic/react-router';
import { help, home, lockClosed, mail, people } from 'ionicons/icons';

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
import AboutUs from './pages/AboutUs';
import Vault from './pages/Vault';
import React from 'react';
import AppAppBar from './components/AppAppBar';
import ApiWrapper from './support/APIWrapper';
import Organization from './pages/Organization';
import OrganizationVault from './pages/OrganizationVault';
import "./support/General.css"
import Register from './pages/Register';

export interface User {
  username: string;
  first_name: string;
  last_name: string;
  email: string;
  phone: string;
}

export const AuthContext = React.createContext(false);
export const UserContext = React.createContext<User | null>(null);

setupIonicReact();

const App: React.FC = () => {
  const [loggedIn, setLoggedIn] = useState(false);
  const [userDetails, setUserDetails] = useState<User | null>(null);

  const checkAuthentication = async () => {
    const response = await ApiWrapper.checkAuthentication()
    if (response){
      setLoggedIn(response.data.is_authenticated);
      fetchUserDetails();
    } 
  };
  

  const fetchUserDetails = async () => {
    const response = await ApiWrapper.fetchUserDetails()
    if (response){
      setUserDetails(response.data.user);
    }
  }

  useEffect(() => {
    checkAuthentication();
  }, []);

  return (
    <UserContext.Provider value={userDetails}>
      <AuthContext.Provider value={loggedIn}>
          <IonApp>
            <IonReactRouter>
              <IonTabs>
                <IonRouterOutlet>
                  <Route exact path="/home">
                    <Home />
                  </Route>
                  <Route exact path="/organizations">
                    <Organizations />
                  </Route>
                  <Route exact path="/organization/:id">
                    <Organization />
                  </Route>
                  <Route exact path="/organization/vault/:id">
                    <OrganizationVault />
                  </Route>
                  <Route exact path="/vault">
                    <Vault/>
                  </Route>
                  <Route exact path="/register">
                    <Register/>
                  </Route>
                  <Route path="/chats">
                    <Chats />
                  </Route>
                  <Route exact path="/aboutUs">
                    <AboutUs />
                  </Route>
                  <Route exact path="/">
                    <Redirect to='/home'></Redirect>
                  </Route>
                </IonRouterOutlet>
                {loggedIn ? (
                  <IonTabBar slot="bottom">
                    <IonTabButton tab="home" href="/home" >
                      <IonIcon aria-hidden="true" icon={home} />
                      <IonLabel>Homepage</IonLabel>
                    </IonTabButton>
                    <IonTabButton tab="organizations" href="/organizations">
                      <IonIcon aria-hidden="true" icon={people} />
                      <IonLabel>Organizations</IonLabel>
                    </IonTabButton>
                    <IonTabButton tab="vault" href="/vault">
                      <IonIcon aria-hidden="true" icon={lockClosed} />
                      <IonLabel>Vault</IonLabel>
                    </IonTabButton>
                    <IonTabButton tab="chats" href="/chats">
                      <IonIcon aria-hidden="true" icon={mail} />
                      <IonLabel>Chats</IonLabel>
                    </IonTabButton>
                  </IonTabBar>
                ) : (
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
    </UserContext.Provider>
  );
};

export default App;
