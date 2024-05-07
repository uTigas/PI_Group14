import { useContext } from "react";
import { AuthContext } from "../App";
import { IonButton, IonCol, IonContent, IonGrid, IonRow, IonTitle } from "@ionic/react";
import ApiWrapper from "../components/APIWrapper";

const Organization: React.FC = (id) => {
    const org = useContext(AuthContext);
    return (
        <IonContent>
            {org ? (
                <IonGrid>
                    <IonRow>
                        <IonCol>
                            <h2>org.name</h2>
                        </IonCol>
                        <IonCol>
                        </IonCol>
                    </IonRow>
                </IonGrid>
            ) : (
                <IonCol className="ion-text-end">
                  <IonButton href={ApiWrapper.backendURI + "login"} color="success">Login</IonButton>
                </IonCol>
            )}
        </IonContent>
    );
};

export default Organization;