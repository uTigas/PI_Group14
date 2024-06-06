import { IonAvatar, IonButton, IonButtons, IonCol, IonContent, IonFooter, IonGrid, IonHeader, IonIcon, IonInput, IonItem, IonList, IonModal, IonRoute, IonRow, IonSegmentButton, IonText, IonTitle, IonToolbar } from "@ionic/react"
import { useEffect, useRef, useState } from "react";
import ApiWrapper from "../../support/APIWrapper";
import { checkmarkOutline, closeOutline } from "ionicons/icons";

const Invites: React.FC<{trigger : string, fetchContacts: () => void, fetchInvites: () => void, invites: any}> = ({trigger, fetchContacts, fetchInvites, invites}) => {
  

    const modal = useRef<HTMLIonModalElement>(null);
    const [error, setError] = useState<boolean>(false);
    const [invited, setInvited] = useState<any>('');

    const inviteChat = async () => {
        if (invited != '') {
            setError(false);
            const formData = new FormData;
            formData.append('username', invited);
            const response = ApiWrapper.inviteChat(formData);
            if (await response) {
                //refresh()
                response.catch(
                    (error) => {
                        setError(true)
                        console.log(error)
                    }
                )
            }
            fetchContacts();
            modal.current?.dismiss();
        }
        else {
            setError(true);
        }
    }

    const acceptInvite = async (id: string) => {
        const formData = new FormData;
        formData.append('invite', id)
        const response = ApiWrapper.acceptChatInvite(formData)
        if (await response)
        {
            fetchInvites()
            fetchContacts()

        }
            //refresh()
    }

    const refuseInvite = async (id: string) => {
        const formData = new FormData;
        formData.append('invite', id)
        const response = ApiWrapper.refuseChatInvite(formData)
        if (await response)
            fetchInvites()
        
            //refresh()
    }

    useEffect(() => {
        fetchInvites();
    }, []); 

    return (
        <IonModal mode='ios' ref={modal} trigger={trigger}>
            <IonHeader>
                <IonToolbar>
                    <IonButtons slot="start">
                        <IonButton onClick={() => modal.current?.dismiss()}>Cancel</IonButton>
                    </IonButtons>
                    <IonTitle>Invites</IonTitle>
                </IonToolbar>
            </IonHeader>
            <IonContent >
              <IonList lines='full'>
                {invites.length > 0 ? invites.map((invite: any) => (
                  <IonItem key={invite.id}>
                    <IonAvatar slot="start">
                      <img src="https://ionicframework.com/docs/img/demos/avatar.svg" alt={`Avatar of ${invite.username}`} />
                    </IonAvatar>
                    <IonText><strong>{invite.inviter_name}</strong> wants to chat!</IonText>
                        <IonButtons slot="end">
                            <IonButton fill='outline' color={'success'} onClick={() => acceptInvite(invite.id)}><IonIcon icon={checkmarkOutline} /></IonButton>
                            <IonButton fill='outline' color={'danger'} onClick={() => refuseInvite(invite.id)}><IonIcon icon={closeOutline} /></IonButton>
                        </IonButtons>
                  </IonItem>
                )) : 
                <>
                <IonItem>
                <IonText>You don't have any pending invites.</IonText>
                </IonItem>
                </>}
                    {error ? (<IonText className='ion-text-center' color={'danger'}>Invalid username or already sent this Invite.</IonText>) : (<></>)}
                </IonList>
            </IonContent>
            <IonFooter>
                <IonGrid>
                    <IonRow>
                        <IonCol>
                            <IonInput className='ion-text-center' placeholder='Enter username...' onIonChange={(e) => setInvited(e.detail.value)}></IonInput>
                        </IonCol>
                        <IonCol size="auto">
                            <IonButton fill='outline' onClick={() => { inviteChat() }}>Send Invite</IonButton>
                        </IonCol>
                    </IonRow>
                </IonGrid>
            </IonFooter>
        </IonModal>
    )
}

export default Invites;
