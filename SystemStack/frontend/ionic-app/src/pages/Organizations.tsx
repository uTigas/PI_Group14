import { IonButton, IonCard, IonCardContent, IonCardHeader, IonCardSubtitle, IonCardTitle, IonCol, IonContent, IonGrid, IonHeader, IonIcon, IonPage, IonRow, IonTitle, IonToolbar } from '@ionic/react';
import { useContext } from 'react';
import { AuthContext, URIContext } from '../App';
import AccessDeniedContainer from '../components/AccessDeniedContainer';
import "./Organizations.css"
import "./General.css"
import { constructOutline, eye } from 'ionicons/icons';
const Organizations: React.FC = () => {

  const logged = useContext(AuthContext);
  const backendURI = useContext(URIContext);

  return (
    <IonPage>
      <IonContent>
        <div>
          {logged ? (
            <IonGrid className='grid'>
              <IonRow>
                <IonCol size-md="5" size="12" className="ion-text-center">
                  <IonTitle className='ion-text-start'>Your Organizations:</IonTitle>
                  <IonCard className="card">
                    <IonCardHeader>
                      <IonCardTitle>DETI</IonCardTitle>
                      <IonCardSubtitle>2 members</IonCardSubtitle>
                    </IonCardHeader>

                    <IonCardContent >
                      <p>Sharing signage templates</p>
                      <IonButton size='small'>Manage<IonIcon className="ion-padding-start" icon={constructOutline}/></IonButton>
                      </IonCardContent>
                  </IonCard>
                </IonCol>
                <IonCol className="ion-text-center">
                <IonTitle className='ion-text-start'>Create a Organization:</IonTitle>

                  <IonCard className="card">
                    <IonCardHeader>
                      <IonCardTitle>DETI</IonCardTitle>
                      <IonCardSubtitle>2 members</IonCardSubtitle>
                    </IonCardHeader>

                    <IonCardContent >
                      <p>Sharing signage templates</p>
                      <IonButton size='small'>Manage<IonIcon className="ion-padding-start" icon={constructOutline}/></IonButton>
                      </IonCardContent>
                  </IonCard>
                </IonCol>
              </IonRow>
            </IonGrid>
          ):(
            <AccessDeniedContainer uri={backendURI}/>
          )}
          </div>
      </IonContent>
    </IonPage>
  );
};

export default Organizations;
